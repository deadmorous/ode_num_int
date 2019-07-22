// SparseJacobianCalculator.h

#ifndef _ALG_SPARSEJACOBIANCALCULATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _ALG_SPARSEJACOBIANCALCULATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#include "../la/SparseMatrixTemplate.h"
#include "./interfaces/VectorMapping.h"
#include <set>
#include <algorithm>

namespace ctm {
namespace math {

template< class VD >
class SparseJacobianCalculator // разреженный Якобиан (с приемущественно нулевыми элементами)
    {
    public:
        typedef VectorTemplate<VD> V; // typedef - присваивание V типа данных VectorTemplate< VD >
        typedef typename V::value_type real_type; // typename указывает на то что value_type - тип данных, тогда real_type - новое имя для этого типа данных
        typedef VectorMapping<VD> Mapping;
        typedef sparse::SparseMatrixTemplate< sparse::SparseMatrixFastData<real_type> > SparseMatrix;
        typedef typename SparseMatrix::Index Index;

        explicit SparseJacobianCalculator( SparseMatrix *J ) : // explicit - не будет выполнятся автоматическое конвертирование
            m_J( J ) // private переменная, шаблона SparseMatrix
            {
            ASSERT( J ); // assert если J = 0, завершает программу
            using namespace std;

            // Initialize the set containing columns to be processed
            auto rows = J->size().first; // -> обращение к члену структуры, член size().first объекта на который указывает J
            auto cols = J->size().second;
            set<unsigned int> colsToPass; // class set содержит упорядоченный набор уникальных объектов типа key
            for( auto col=0u; col<cols; ++col )
                colsToPass.insert(col); // colsToPass с помощью insert заполнаяется столбцами

            // Compute the numbers of nonzero elements in each column
            vector<unsigned int> colSizes( cols, 0 ); // colSizes (size_type,value_type)
            for( const auto& e : *J ) // Указатель (*J) получает адрес переменной (&e), ":" - указывает компилятору что инициализировать
                ++colSizes[e.first.second];

            // Compute column layouts (for each column, find the
            // numbers and element addresses of all rows containing nonzero elements)
            vector< vector<RP> > colRows( cols ); // RP - структура, которая хранит два разнородных объекта, как единое целое (с помощью std::pair)
            for( auto col=0u; col<cols; ++col ) {
                colRows[col].resize( colSizes[col] );
                colSizes[col] = 0;
                }
            for( auto& e : *J ) {
                auto col = e.first.second;
                colRows[col][colSizes[col]++] = RP( e.first.first, &e.second ); // шаблон pair; first - row number, second - adress ??
                }

            auto addRows = [&colRows]( vector<bool>& affectedRows, unsigned int col ) { // ???
                for( const auto& rp : colRows[col] )
                    affectedRows[rp.first] = true;
                };

            auto intersects = [&colRows]( const vector<bool>& affectedRows, unsigned int col ) {
                for( const auto& rp : colRows[col] )
                    if( affectedRows[rp.first] )
                        return true;
                return false;
                };

            while( !colsToPass.empty() ) {
                m_layout.push_back( MulticolEvalData() ); // push_back - добавление элемента в конец вектора
                auto& multicolData = *m_layout.rbegin(); // rbegin - возвращает обратный(обходит контейнер от последнего элемента к первому) итератор на первый элемент
                auto col = *colsToPass.begin();
                colsToPass.erase( colsToPass.begin() ); // удаляет элементы
                vector<bool> affectedRows( rows, false );
                addRows( affectedRows, col );
                multicolData.push_back( C2R( col, colRows[col] ) ); // C2R шаблон pair; first - col number, second - set of nonzero elements
                for( auto itcol=colsToPass.begin(); itcol!=colsToPass.end(); ) {
                    auto col2 = *itcol;
                    if( intersects( affectedRows, col2 ) )
                        ++itcol;
                    else {
                        multicolData.push_back( C2R( col2, colRows[col2] ) );
                        addRows( affectedRows, col2 );
                        itcol = colsToPass.erase( itcol );
                        }
                    }
                }
            }

        void calculate( const VectorMapping<VD>& mapping, const V& x0, real_type delta ) const // VectorMapping - шаблон класса интерфейс для векторной функции
            {                                                                                  // V - шаблон типа данных VectorTemplate
            auto n = mapping.inputSize(); // inputSize - virtual unsigned int inputSize() const = 0; ???
            ASSERT( m_J->size().first == mapping.outputSize() ); // mapping.outputSize() const = 0
            ASSERT( m_J->size().second == n ); // size().second = cols
            V x(n), f0(n), f(n); // 3 векторных функции
            mapping.map( f0, x0 ); // virtual void map( V& dst, const V& x ) const = 0; V(VectorTemplate) определяет вектор и операции над ним
            for( const auto& multicolData : m_layout ) { // m_layout - information about how to compute all columns of Jacobian, grouped in multi-column data
                x = x0;
                for( const auto& c2r : multicolData )
                    x[c2r.first] += delta; // c2r.first : col number = col numberr + delta
                mapping.map( f, x ); // два параметра?
                for( const auto& c2r : multicolData ) {
                    for( const auto& rp : c2r.second )
                        *rp.second = ( f[rp.first] - f0[rp.first] ) / delta; // rp.second : address of el in Jac; f[rp.first] - f0[rp.first] : du, delta = dx??
                    }
                }
            }

        unsigned int mappingEvaluationsRequired() const {   // ???
            return m_layout.size() + 1;
            }

    private:
        SparseMatrix *m_J;
        // First element is a row number,
        // second element is the address of element in the Jacobian
        typedef std::pair< unsigned int, real_type* > RP;
        // First element is a column number,
        // second element is the set of potentially nonzero Jacobian rows in that column
        typedef std::pair<unsigned int, std::vector<RP> > C2R;

        // Each mapping evaluation could be used for computing more than one column of the Jacobian,
        // if its sparse structure allows it. In this array, there is information about all columns
        // to be computed within a single mapping evaluation.
        typedef std::vector< C2R > MulticolEvalData;

        // Information about how to compute all columns of the Jacobian, grouped by multi-column
        // data
        typedef std::vector< MulticolEvalData > JacobianEvalData;

        JacobianEvalData m_layout;
    };

template< class VD >
inline void computeSparseJacobian(
        sparse::SparseMatrixTemplate< sparse::SparseMatrixFastData< typename VD::value_type > >& J,
        const VectorMapping<VD>& mapping,
        const VectorTemplate<VD>& x0, typename VD::value_type delta )
    {
    SparseJacobianCalculator<VD>( &J ).calculate( mapping, x0, delta );
    }

} // end namespace math
} // end namespace ctm

#endif // _ALG_SPARSEJACOBIANCALCULATOR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
