// cxx_mock_ptr.h

/// \file
/// \brief Declares the ctm::cxx::mock_ptr template class.

#ifndef _INFRA_CXX_MOCK_PTR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _INFRA_CXX_MOCK_PTR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

namespace ctm {
namespace cxx {

/// \brief Class that stores an element of type T thus being able to provide its address.
///
/// This template class is needed when a pointer to a value is required, but
/// the value is temporary. In this case, one can use an instance of this
/// class instead of a raw pointer. One typical use case is to specify mock_ptr
/// as the 4-th parameter (pointer) for an instantiation of the std::iterator template,
/// in the case when the iterator's dereferencing operator returns a temporary value.
template< class T >
class mock_ptr {
    public:
        /// \brief Constructor.
        /// \param data Value to be copied and stored in a private field.
        explicit mock_ptr( const T& data ) : m_data( data ) {}

        /// \brief Returns a reference to the private field.
        T& operator*() {
            return m_data;
            }

        /// \brief Returns a constant reference to the private field.
        const T& operator*() const {
            return m_data;
            }

        /// \brief Returns a pointer to the private field.
        T *operator->() {
            return &m_data;
            }

        /// \brief Returns a constant pointer to the private field.
        const T *operator->() const {
            return &m_data;
            }

        /// \brief Implicit cast to pointer to T*.
        operator T*() const {
            return &m_data;
            }
    private:
        T m_data;
    };

} // end namespace cxx
} // end namespace ctm

#endif // _INFRA_CXX_MOCK_PTR_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
