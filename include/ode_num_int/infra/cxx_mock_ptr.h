// cxx_mock_ptr.h

#ifndef _INFRA_CXX_MOCK_PTR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _INFRA_CXX_MOCK_PTR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

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

#endif // _INFRA_CXX_MOCK_PTR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
