#include "cute.h"
#include "seq.hpp"
#include "sequence.hpp"
#include "nucl.hpp"
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <string>

typedef unsigned long long ull;

void TestSeqSelector() {
	ASSERT_EQUAL('G', nucl(Seq<10>("ACGTACGTAC")[2]));
	ASSERT_EQUAL('G', nucl(Seq<60,ull>("ACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTAC")[2]));
	ASSERT_EQUAL('G', nucl(Seq<60,ull>("ACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTAC")[16]));
	ASSERT_EQUAL('T', nucl(Seq<60,ull>("ACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTAC")[17]));
	ASSERT_EQUAL('A', nucl(Seq<60,ull>("ACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTAC")[18]));
	ASSERT_EQUAL('C', nucl(Seq<60,ull>("ACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTAC")[19]));
}

void TestSeqShiftLeft() {
	Seq<10> s("ACGTACGTAC");
	ASSERT_EQUAL("CGTACGTACA", (s << dignucl('A')).str());
	ASSERT_EQUAL("CGTACGTACC", (s << dignucl('C')).str());
	ASSERT_EQUAL("CGTACGTACG", (s << dignucl('G')).str());
	ASSERT_EQUAL("CGTACGTACT", (s << dignucl('T')).str());
	Seq<60,ull> s2("ACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTAC");
	ASSERT_EQUAL("CGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACA", (s2 << dignucl('A')).str());
	ASSERT_EQUAL("CGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACC", (s2 << dignucl('C')).str());
	ASSERT_EQUAL("CGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACG", (s2 << dignucl('G')).str());
	ASSERT_EQUAL("CGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACT", (s2 << dignucl('T')).str());
}

void TestSeqShiftRight() {
	Seq<10> s("ACGTACGTAC");
	ASSERT_EQUAL("AACGTACGTA", (s >> dignucl('A')).str());
	ASSERT_EQUAL("CACGTACGTA", (s >> dignucl('C')).str());
	ASSERT_EQUAL("GACGTACGTA", (s >> dignucl('G')).str());
	ASSERT_EQUAL("TACGTACGTA", (s >> dignucl('T')).str());
	Seq<60,ull> s2("ACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTAC");
	ASSERT_EQUAL("AACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTA", (s2 >> dignucl('A')).str());
	ASSERT_EQUAL("CACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTA", (s2 >> dignucl('C')).str());
	ASSERT_EQUAL("GACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTA", (s2 >> dignucl('G')).str());
	ASSERT_EQUAL("TACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTA", (s2 >> dignucl('T')).str());
	ASSERT(Seq<5>("ACACA") == Seq<5>("CACAC")>>dignucl('A'));
}

void TestSeqStr() {
	Seq<10> s("ACGTACGTAC");
	ASSERT_EQUAL("ACGTACGTAC", s.str());
	Seq<60,ull> s2("ACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTAC");
	ASSERT_EQUAL("ACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTAC", s2.str());
}

void TestSeqHeadAndTail() {
	Seq<10> s("ACGTACGTAC");
	ASSERT_EQUAL("CGTACGTAC", Seq<9>(s, 1).str()); // tail
	ASSERT_EQUAL("ACGTACGTA", Seq<9>(s).str()); // head
}

void TestSeqFromBiggerSeq() {
	Seq<10> s("ACGTACGTAC");
	ASSERT_EQUAL("ACGTA", Seq<5>(s).str());
}

void TestSeqFromType() {
	Sequence s("ACGTACGTAC");
	ASSERT_EQUAL("ACGTA", Seq<5>(s).str());
	ASSERT_EQUAL("GTACG", Seq<5>(s, 2).str());
}

void TestSeqPushBack() {
	{
		Seq<4> s("ACGT");
		ASSERT_EQUAL("ACGTC", s.pushBack('C').str());
	}
	{
		Seq<4, unsigned char> s("ACGT");
		ASSERT_EQUAL("ACGTC", s.pushBack('C').str());
	}
	{
		Seq<3> s("ACG");
		ASSERT_EQUAL("ACGC", s.pushBack('C').str());
	}
}

//template <size_t k>
//Seq<k - 1> TemplateEnd(const Seq<k> &s)
//{
//	return s.end( );

//	boost::function<Seq<k-1>()> foo = boost::bind(&Seq<k>::end<k-1>, s);
//
//return foo(); s.end<(k-1)>();
	//return /*Seq<k-1>();  */s.size();//end<k-1, unsigned int>();
//}

//template <size_t k>
//Seq<k-1> TemplateEnd2(const Sequence &s) {
//	return s.end<k-1>();
//}

//void TestTemplateSeqEnd() {
//	Seq<5> s("ACGTA");
//	ASSERT_EQUAL("CGTA", TemplateEnd<5>(s).str());
//}

//void TestTemplateSequenceEnd() {
//	Sequence s("ACGTA");
//	ASSERT_EQUAL("CGTA", TemplateEnd<5>(s).str());
//}

void TestSeqEnd() {
	Seq<5> s1("ACGTA");
	ASSERT_EQUAL("CGTA", s1.end<4>().str());
}

void TestSeqStart() {
	Seq<5> s1("ACGTA");
	ASSERT_EQUAL("ACGT", s1.start<4>().str());
}

void TestSeqComplex() {
	Sequence s1("ACAAA");
	Sequence s2("CAAAC");
	ASSERT_EQUAL((!(Seq<4>(!s1))).str(), Seq<4>(s2).str());
	ASSERT_EQUAL(!(Seq<4>(!s1)), Seq<4>(s2));
}

void TestSeqFromCharArray() {
	std::string s = "ACGTACGTAC";
	ASSERT_EQUAL(Seq<10>(s.c_str()).str(), "ACGTACGTAC");
}

void TestSeqReverseComplement() {
	Seq<10> s("ACGTACGTAC");
	ASSERT_EQUAL("GTACGTACGT", (!s).str());
	Seq<60,ull> s2("ACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTACACGTACGTAC");
	ASSERT_EQUAL("GTACGTACGTGTACGTACGTGTACGTACGTGTACGTACGTGTACGTACGTGTACGTACGT", (!s2).str());
	Seq<9> s3("ACGTACGTA");
	ASSERT_EQUAL("TACGTACGT", (!s3).str());
}

void Test16() {
	Seq<16> s("AAAAAAAAAAAAAAAA");
	ASSERT_EQUAL(s << 'C', Seq<16>("AAAAAAAAAAAAAAAC"));
}

void Test16_2() {
	Seq<16> s("TTTTTTTTTTTTTTTT");
	ASSERT_EQUAL(Seq<16>("TTTTTTTTTTTTTTTA"), s << 'A');
}

template <int k>
struct A
{
	A foo() const
	{
		return A<1>();
	}

	template<int j>
	A<j> bar() const
	{
		return A<j>();
	}

};

void foobar(A<4> const& a)
{
	A<3> aa = a.bar<3>();
}


cute::suite SeqSuite(){
	cute::suite s;
	s.push_back(CUTE(TestSeqSelector));
	s.push_back(CUTE(TestSeqStr));
	s.push_back(CUTE(TestSeqFromCharArray));
	s.push_back(CUTE(TestSeqFromBiggerSeq));
	s.push_back(CUTE(TestSeqFromType));
	s.push_back(CUTE(TestSeqStart));
	s.push_back(CUTE(TestSeqEnd));

//	s.push_back(CUTE(TestTemplateSeqEnd));
//	s.push_back(CUTE(TestTemplateSequenceEnd));

	s.push_back(CUTE(TestSeqShiftLeft));
	s.push_back(CUTE(TestSeqShiftRight));
	s.push_back(CUTE(TestSeqPushBack));
	s.push_back(CUTE(TestSeqHeadAndTail));
	s.push_back(CUTE(TestSeqReverseComplement));
	s.push_back(CUTE(TestSeqComplex));
	s.push_back(CUTE(Test16));
	s.push_back(CUTE(Test16_2));
	return s;
}

