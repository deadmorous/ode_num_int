// cxx_mock_ptr.h

#ifndef _CXX_MOCK_PTR_H_108D9C57_FA48_4FDA_865B_79A025682165
#define _CXX_MOCK_PTR_H_108D9C57_FA48_4FDA_865B_79A025682165

namespace ctm {
namespace cxx {

template< class T >
class mock_ptr {
    public:
        explicit mock_ptr( const T& data ) : m_data( data ) {}
        T& operator*() {
            return m_data;
            }
        const T& operator*() const {
            return m_data;
            }
        T *operator->() {
            return &m_data;
            }
        const T *operator->() const {
            return &m_data;
            }
        operator T*() const {
            return &m_data;
            }
    private:
        T m_data;
    };

} // end namespace cxx
} // end namespace ctm

#endif // _CXX_MOCK_PTR_H_108D9C57_FA48_4FDA_865B_79A025682165
