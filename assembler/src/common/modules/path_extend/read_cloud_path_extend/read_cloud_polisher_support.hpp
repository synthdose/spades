#pragma once

#include "read_cloud_connection_conditions.hpp"
#include "simple_graph.hpp"

namespace path_extend {
    class SupportedEdgesGraphExtractor {
        const Graph& g_;
        shared_ptr<LongEdgePairGapCloserPredicate> read_cloud_predicate_;

     public:
        typedef path_extend::SimpleGraph<EdgeId> SimpleGraph;

        SupportedEdgesGraphExtractor(const Graph &g_,
                                     const shared_ptr<LongEdgePairGapCloserPredicate> &read_cloud_predicate_);

        set<EdgeId> ExtractSupportedEdges(const EdgeId start, const EdgeId end) const;

        DECL_LOGGER("SupportedEdgesGraphExtractor");
    };
}