#ifndef SHOT_DATA_LISTENER_H
#define SHOT_DATA_LISTENER_H

#include "Data.h"

namespace OpenConnectV1 {
    class ShotDataListener {
    public:
        virtual ~ShotDataListener() = default;
        virtual void onShotDataReceived(const OpenConnectV1::ShotData& shotData) = 0;
    };

}

#endif
