// defs.h

#ifndef _DEFS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
#define _DEFS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_

#if defined(_WIN32) && defined(_MSC_VER)

#ifdef ode_num_int_EXPORTS
#define ODE_NUMINT_API __declspec(dllexport)
#else // ode_num_int_EXPORTS
#define ODE_NUMINT_API __declspec(dllimport)
#endif // ode_num_int_EXPORTS
#else // defined(_WIN32) && defined(_MSC_VER)
#define ODE_NUMINT_API
#endif // defined(_WIN32) && defined(_MSC_VER)

#define CTM_FACTORY_REGISTER_TYPE_FOR_COMMON_VD( ClassTemplateName, name ) \
    typedef ClassTemplateName< ctm::math::VectorData<double> > General##ClassTemplateName; \
    CTM_FACTORY_REGISTER_TYPE( General##ClassTemplateName, name )

#endif // _DEFS_H_AB0B81B0_CF3E_424f_9766_BA04D388199F_
