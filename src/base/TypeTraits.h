//
// Created by jun on 18-3-6.
//

#ifndef ZLPP_TYPETRAITS_H
#define ZLPP_TYPETRAITS_H

namespace zh {
    namespace {
        template<bool, class Ta, class Tb>
        struct IfThenElse;

        template<class Ta, class Tb>
        struct IfThenElse<true, Ta, Tb> {
            using result = Ta;
        };

        template<class Ta, class Tb>
        struct IfThenElse<false, Ta, Tb> {
            using result = Tb;
        };
    }

    struct _true_type {};
    struct _false_type {};

    /*
     * 萃取传入的Ｔ类型的类型特性
     */
    template<class T>
    struct _type_traits
    {
        typedef _false_type
    };
}
#endif //ZLPP_TYPETRAITS_H
