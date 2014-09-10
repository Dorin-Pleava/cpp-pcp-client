#ifndef CTHUN_CLIENT_SRC_CONNECTION_MANAGER_H_
#define CTHUN_CLIENT_SRC_CONNECTION_MANAGER_H_

#include "client.h"
#include "endpoint.h"
#include "connection.h"

#include <vector>
#include <string>
#include <memory>

namespace Cthun {
namespace Client {

//
// CONNECTION_MANAGER singleton
//

#define CONNECTION_MANAGER ConnectionManager::Instance()

class ConnectionManager {
  public:
    static ConnectionManager& Instance();

    /// Throw a client_error in case the endpoint has already started
    /// or in case of a missing certificate file.
    void configureSecureEndpoint(const std::string& ca_crt_path,
                                 const std::string& client_crt_path,
                                 const std::string& client_key_path);

    /// Attempt to close all connections (message errors will be
    /// logged and filtered) and deletes the Endpoint instance.
    /// Throws a message_error in case it fails to close a connection.
    void resetEndpoint();

    /// Return a pointer to a new connection.
    /// The connection will not be opened.
    /// The connection ID will not be stored.
    Connection::Ptr createConnection(std::string url);

    /// Starts the pointed connection.
    /// Throw a connection_error in case of failure.
    void open(Connection::Ptr connection_ptr);

    /// Send message on the pointed connection.
    /// Throw a message_error in case of failure.
    void send(Connection::Ptr connection_ptr, Message message);

    /// Close the pointed connection; reason and code are optional
    /// (respectively default to "Closed by client" and normal as
    /// in rfc6455).
    /// Throw a message_error in case of failure.
    void close(Connection::Ptr connection_ptr,
               Close_Code code = Close_Code_Values::normal,
               Message reason = DEFAULT_CLOSE_REASON);

    /// Close all connections that are in 'open' state.
    /// Reset the list of connections managed by the endpoint.
    /// Throw a message_error in case of failure.
    void closeAllConnections();

    /// Return the IDs of the connections managed by the endpoint,
    /// regardless of their state.
    std::vector<Connection_ID> getConnectionIDs();


  private:
    Endpoint::Ptr endpoint_ptr_;
    bool is_secure_ { false };
    std::string ca_crt_path_;
    std::string client_crt_path_;
    std::string client_key_path_;

    // To verify the Endpoint instantiation
    bool endpoint_running_;

    ConnectionManager();

    void startEndpointIfNeeded();
};

}  // namespace Client
}  // namespace Cthun

#endif  // CTHUN_CLIENT_SRC_CONNECTION_MANAGER_H_
