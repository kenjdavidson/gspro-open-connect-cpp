#ifndef STATUS_LISTENER_H
#define STATUS_LISTENER_H

namespace OpenConnectV1 {
    enum class ServerStatus {
        Disconnected = 0,
        Listening = 1,
        Connected = 2
    };

    class StatusListener {
    public:
        virtual ~StatusListener() = default;

        virtual void onStatusChanged(ServerStatus status) = 0;
    };
}

#endif