//
// Created by andrey on 04.12.15.
//

#include "read_cloud_path_extend/read_cloud_dijkstras.hpp"
#include "scaffold_graph_constructor.hpp"

namespace path_extend {
namespace scaffold_graph {


void BaseScaffoldGraphConstructor::ConstructFromEdgeConditions(func::TypedPredicate<EdgeId> edge_condition,
                                                               const vector<shared_ptr<ConnectionCondition>> &connection_conditions,
                                                               bool use_terminal_vertices_only) {
    for (auto e = graph_->AssemblyGraph().ConstEdgeBegin(); !e.IsEnd(); ++e) {
        if (edge_condition(*e)) {
            graph_->AddVertex(*e);
        }
    }
    ConstructFromConditions(connection_conditions, use_terminal_vertices_only);
}

void BaseScaffoldGraphConstructor::ConstructFromSet(const set<EdgeId> edge_set,
                                                    const vector<shared_ptr<ConnectionCondition>> &connection_conditions,
                                                    bool use_terminal_vertices_only) {
    graph_->AddVertices(edge_set);
    TRACE("Added vertices");
    ConstructFromConditions(connection_conditions, use_terminal_vertices_only);
}

void BaseScaffoldGraphConstructor::ConstructFromConditions(const vector<shared_ptr<ConnectionCondition>> &connection_conditions,
                                                       bool use_terminal_vertices_only) {
//TODO :: awful. It depends on ordering of connected conditions.
    TRACE("Constructing from conditions");
    TRACE(connection_conditions.size() << " conditions.");
    TRACE(connection_conditions.back()->IsLast());
    for (auto condition : connection_conditions) {
        TRACE("Checking condition");
        if (condition->IsLast())
            ConstructFromSingleCondition(condition, true);
        else
            ConstructFromSingleCondition(condition, use_terminal_vertices_only);
    }
}

void BaseScaffoldGraphConstructor::ConstructFromSingleCondition(const shared_ptr<ConnectionCondition> condition,
                                                            bool use_terminal_vertices_only) {
    TRACE("Terminal only: " << (use_terminal_vertices_only ? "True" : "False"));
    for (const auto& v : graph_->vertices()) {
        TRACE("Vertex " << graph_->int_id(v));

        if (use_terminal_vertices_only && graph_->OutgoingEdgeCount(v) > 0)
            continue;

        //fixme connection conditions for paths?
        EdgeGetter getter;
        EdgeId e = getter.GetEdgeFromScaffoldVertex(v);
        auto connected_with = condition->ConnectedWith(e);
        for (const auto& pair : connected_with) {
            EdgeId connected = pair.first;
            double w = pair.second;
            TRACE("Connected with " << graph_->int_id(connected));
            if (graph_->Exists(connected)) {
                if (use_terminal_vertices_only && graph_->IncomingEdgeCount(connected) > 0)
                    continue;
                graph_->AddEdge(e, connected, condition->GetLibIndex(), w, 0);
            }
        }
    }
}

shared_ptr<ScaffoldGraph> SimpleScaffoldGraphConstructor::Construct() {
    ConstructFromSet(edge_set_, connection_conditions_);
    return graph_;
}

shared_ptr<ScaffoldGraph> DefaultScaffoldGraphConstructor::Construct() {
    ConstructFromSet(edge_set_, connection_conditions_);
    ConstructFromEdgeConditions(edge_condition_, connection_conditions_);
    return graph_;
}

PredicateScaffoldGraphConstructor::PredicateScaffoldGraphConstructor(const Graph& assembly_graph,
                                                                     const ScaffoldGraph& old_graph_,
                                                                     shared_ptr<EdgePairPredicate> predicate_,
                                                                     size_t max_threads)
    : BaseScaffoldGraphConstructor(assembly_graph), old_graph_(old_graph_),
      predicate_(predicate_), max_threads_(max_threads) {}

void PredicateScaffoldGraphConstructor::ConstructFromGraphAndPredicate(const ScaffoldGraph& old_graph,
                                                                       shared_ptr<EdgePairPredicate> predicate) {
    for (const auto& vertex: old_graph.vertices()) {
        graph_->AddVertex(vertex);
    }
    vector<ScaffoldGraph::ScaffoldEdge> scaffold_edges;
    for (const auto& edge: old_graph.edges()) {
        scaffold_edges.push_back(edge);
    }
    size_t counter = 0;
    const size_t block_size = scaffold_edges.size() / 10;
    size_t threads = max_threads_;
#pragma omp parallel for num_threads(threads)
    for (size_t i = 0; i < scaffold_edges.size(); ++i) {
        auto edge = scaffold_edges[i];
        bool check_predicate = (*predicate)(edge);
#pragma omp critical
        {
            if (check_predicate) {
                graph_->AddEdge(edge);
            }
            ++counter;
            if (counter % block_size == 0) {
                INFO("Processed " << counter << " edges out of " << scaffold_edges.size());
            }
        }
    }
}

shared_ptr<ScaffoldGraph> PredicateScaffoldGraphConstructor::Construct() {
    ConstructFromGraphAndPredicate(old_graph_, predicate_);
    return graph_;
}
ScoreFunctionScaffoldGraphConstructor::ScoreFunctionScaffoldGraphConstructor(const Graph& assembly_graph,
                                                                             const ScaffoldGraph& old_graph_,
                                                                             shared_ptr<EdgePairScoreFunction> score_function_,
                                                                             const double score_threshold, size_t num_threads)
    : BaseScaffoldGraphConstructor(assembly_graph), old_graph_(old_graph_),
      score_function_(score_function_), score_threshold_(score_threshold), num_threads_(num_threads) {}

void ScoreFunctionScaffoldGraphConstructor::ConstructFromGraphAndScore(const ScaffoldGraph& graph,
                                                                       shared_ptr<EdgePairScoreFunction> score_function,
                                                                       double score_threshold, size_t threads) {
    //fixme score overwrites previous weight!
    for (const auto& vertex: graph.vertices()) {
        graph_->AddVertex(vertex);
    }
    //fixme switch to tbb or use chunk splitter
    vector<ScaffoldGraph::ScaffoldEdge> scaffold_edges;
    for (const auto& edge: graph.edges()) {
        scaffold_edges.push_back(edge);
    }
    size_t counter = 0;
    const size_t block_size = scaffold_edges.size() / 10;
    #pragma omp parallel for num_threads(threads)
    for (size_t i = 0; i < scaffold_edges.size(); ++i) {
        ScaffoldGraph::ScaffoldEdge edge = scaffold_edges[i];
        double score = score_function->GetScore(edge);
    #pragma omp critical
        {
            TRACE("Adding edge");
            if (math::ge(score, score_threshold)) {
                graph_->AddEdge(edge.getStart(), edge.getEnd(), edge.getColor(), score, edge.getLength());
            }
            TRACE("Edge added");
            ++counter;
            if (counter % block_size == 0) {
                INFO("Processed " << counter << " edges out of " << scaffold_edges.size());
            }
        }
    }
}
shared_ptr<ScaffoldGraph> ScoreFunctionScaffoldGraphConstructor::Construct() {
    ConstructFromGraphAndScore(old_graph_, score_function_, score_threshold_, num_threads_);
    return graph_;
}
shared_ptr<ScaffoldGraph> UniqueScaffoldGraphConstructor::Construct() {
    INFO("Scaffolding distance: " << distance_);
    auto dij = omnigraph::CreateUniqueDijkstra(graph_->AssemblyGraph(), distance_, unique_storage_);
    //        auto bounded_dij = DijkstraHelper<Graph>::CreateBoundedDijkstra(g_, distance_, 10000);

    for (const auto unique_edge: unique_storage_) {
        graph_->AddVertex(unique_edge);
    }
    size_t counter = 0;
    const size_t block_size = unique_storage_.size() / 10;
    for (const auto unique_edge: unique_storage_) {
        dij.Run(graph_->AssemblyGraph().EdgeEnd(unique_edge));
        for (auto v: dij.ReachedVertices()) {
            size_t distance = dij.GetDistance(v);
            if (distance < distance_) {
                for (auto connected: graph_->AssemblyGraph().OutgoingEdges(v)) {
                    if (unique_storage_.IsUnique(connected) and connected != unique_edge
                        and connected != graph_->AssemblyGraph().conjugate(unique_edge)) {
                        graph_->AddEdge(unique_edge, connected, (size_t) -1, 0, distance);
                    }
                }
            }
        }
        ++counter;
        if (counter % block_size == 0) {
        }
    }
    return graph_;
}
UniqueScaffoldGraphConstructor::UniqueScaffoldGraphConstructor(
    const Graph& assembly_graph,
    const ScaffoldingUniqueEdgeStorage& unique_storage_,
    const size_t distance_)
    : BaseScaffoldGraphConstructor(assembly_graph), unique_storage_(unique_storage_), distance_(distance_) {}
} //scaffold_graph
} //path_extend