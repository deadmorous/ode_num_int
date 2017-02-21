// vector_for_ode_solver.h

#ifndef _VECTOR_FOR_ODE_SOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _VECTOR_FOR_ODE_SOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "vector_util.h"
#include "equ_num_solv_01/m_ode_solver.h"
#include <cstddef>

namespace ctm {
namespace math {

template< class ElementType >
class VectorDataForOdeSolver
    {
    public:
        typedef VectorDataForOdeSolver< ElementType > vector_data_type;
        typedef ElementType value_type;
        typedef WRCIArrayX< Array_Vector< ElementType > > Data;
        typedef size_t size_type;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef value_type *iterator;
        typedef const value_type *const_iterator;

        VectorDataForOdeSolver() {}
        VectorDataForOdeSolver( size_type size ) {
            m_data.make( size );
            }

        VectorDataForOdeSolver( const value_type *data, size_type size ) {
            m_data.make( size );
            std::copy( data, data + size, m_data.GetContainer().data() );
            }

        VectorDataForOdeSolver( const vector_data_type& that ) {
            assign( that );
            }

        VectorDataForOdeSolver( vector_data_type&& that ) {
            std::swap( m_data, that.m_data );
            }

        vector_data_type& operator=( const vector_data_type& that )
            {
            if( this != &that )
                assign( that );
            return *this;
            }

        vector_data_type& operator=( vector_data_type&& that )
            {
            if( this != &that )
                std::swap( m_data, that.m_data );
            return *this;
            }

        void swap( vector_data_type& that ) {
            std::swap( m_data, that.m_data );
            }

        reference operator[]( unsigned int index ) {
            return m_data.GetContainer()[ index ];
            }

        const_reference operator[]( unsigned int index ) const {
            return m_data.GetContainer()[ index ];
            }

        reference at( unsigned int index ) {
            return m_data.GetContainer()[ index ];
            }

        const_reference at( unsigned int index ) const {
            return m_data.GetContainer()[ index ];
            }

        iterator begin() {
            return m_data.GetContainer().data();
            }
        const_iterator cbegin() const {
            return m_data.GetContainer().data();
            }
        const_iterator begin() const {
            return m_data.GetContainer().data();
            }

        iterator end() {
            return m_data.GetContainer().data() + m_data.GetContainer().size();
            }
        const_iterator cend() const {
            return m_data.GetContainer().data() + m_data.GetContainer().size();
            }
        const_iterator end() const {
            return m_data.GetContainer().data() + m_data.GetContainer().size();
            }

        void resize( unsigned int size ) {
            m_data.make( size );
            }

        void clear() {
            std::fill( begin(), end(), value_type() );
            }

        size_type size() const {
            if( auto container = &m_data.GetContainer() )
                return container->size();
            else
                return 0;
            }

        Data& data() {
            return m_data;
            }

        template< class ThatD >
        void assign( const ThatD& that )
            {
            resize( that.size() );
            std::copy( that.begin(), that.end(), begin() );
            }

    private:
        mutable Data m_data;
    };

template< class T >
inline VectorTemplate< VectorDataForOdeSolver< T > > vec( const WRCIArrayX< Array_Vector< T > >& d ) {
    return VectorTemplate< VectorDataForOdeSolver< T > >( VectorDataForOdeSolver< T >( d ) );
    }

template< class T >
inline VectorTemplate< VectorDataForOdeSolver< T > > vec( size_t size ) {
    return VectorTemplate< VectorDataForOdeSolver< T > >( size );
    }

template< class T >
inline VectorTemplate< VectorDataForOdeSolver< T > > vec() {
    return VectorTemplate< VectorDataForOdeSolver< T > >();
    }

template< class T >
inline VectorTemplate< VectorDataForOdeSolver< T > > vec( const T *data, size_t size ) {
    return VectorTemplate< VectorDataForOdeSolver< T > >( VectorDataForOdeSolver< T >( data, size ) );
    }

typedef VectorDataForOdeSolver< double > VDSimDyn;

} // end namespace math
} // end namespace ctm

#define CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( ClassTemplateName, name ) \
    typedef ClassTemplateName< ctm::math::VDSimDyn > SimDyn##ClassTemplateName; \
    CTM_FACTORY_REGISTER_TYPE( SimDyn##ClassTemplateName, name ) \
    typedef ClassTemplateName< ctm::math::VectorData<double> > General##ClassTemplateName; \
    CTM_FACTORY_REGISTER_TYPE( General##ClassTemplateName, name )

#endif // _VECTOR_FOR_ODE_SOLVER_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
