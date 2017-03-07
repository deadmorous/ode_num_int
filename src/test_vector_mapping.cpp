// test_vector_mapping.cpp

#include "ode_num_int/VectorNarrowingMapping.h"
#include <iostream>

using namespace std;
using namespace ctm;
using namespace math;

namespace {

typedef VectorData< double > VD;
typedef Vector< double > V;

class MyMapping : public VectorMapping<VD>
    {
    public:
        explicit MyMapping( unsigned int n ) : n(n) {}

        unsigned int inputSize() const {
            return 2*n;
            }

        unsigned int outputSize() const {
            return n;
            }

        void map( V& dst, const V& x ) const {
            ASSERT( x.size() == 2*n );
            dst.resize( n );
            for( unsigned int i=0; i<n; ++i )
                dst[i] = (i+1)*(x[2*i] + x[2*i+1]);
            }

        vector<unsigned int> inputIds() const
            {
            vector<unsigned int> result( 2*n );
            for( unsigned int i=0; i<2*n; ++i )
                result[i] = i;
            return result;
            }

        vector<unsigned int> outputIds() const
            {
            vector<unsigned int> result( n );
            for( unsigned int i=0; i<n; ++i )
                result[i] = 1000 + i;
            return result;
            }

    private:
        unsigned int n;
    };

template< class S, class T >
inline S& operator<<( S& s, const vector<T>& x )
    {
    s << '[';
    for( unsigned int i=0, n=x.size(); i<n; ++i ) {
        if( i > 0 )
            s << ", ";
        s << x[i];
        }
    s << ']';
    return s;
    }

void testVectorMapping()
    {
    unsigned int n = 10;
    auto mm = make_shared< MyMapping >( n );
    VectorMapping<VD>& m = *mm;
    V x( m.inputSize() );
    fill( x.begin(), x.end(), 1 );
    cout << "Input: " << x << endl;
    cout << "Output: " << m(x) << endl;
    cout << "Input ids: " << m.inputIds() << endl;
    cout << "Output ids: " << m.outputIds() << endl;

    cout << endl;
    VectorNarrowingMapping<VD> nmm;
    nmm.setMapping( mm );
    vector<unsigned int> disabledInput = {1,3,5};
    vector<unsigned int> disabledOutput = {6,7,8,9};
    nmm.setInputNarrowing( disabledInput, x );
    nmm.setOutputNarrowing( disabledOutput );
    cout << "Disabled input: " << disabledInput << endl;
    cout << "Disabled output: " << disabledOutput << endl;
    VectorMapping<VD>& nm = nmm;
    V xn( nm.inputSize() );
    fill( xn.begin(), xn.end(), 2 );
    cout << "Input: " << xn << endl;
    cout << "Output: " << nm(xn) << endl;
    cout << "Input ids: " << nm.inputIds() << endl;
    cout << "Output ids: " << nm.outputIds() << endl;
    }

// Uncomment to call the test at startup
// struct _{_() { testVectorMapping(); } }__;

} // anonymous namespace
