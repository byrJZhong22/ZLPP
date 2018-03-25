//
// Created by jun on 18-3-2.
//

#ifndef ZLPP_COPYABLE_H
#define ZLPP_COPYABLE_H

namespace zh {

    // A tag class emphasises the objects are copyable.
    // The empty base class optimization applies.
    // Any derived class of copyable should be a value type.
    class copyable {
    };
}

#endif //ZLPP_COPYABLE_H
