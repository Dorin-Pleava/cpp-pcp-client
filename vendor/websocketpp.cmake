INCLUDE(ExternalProject)

# Add an external project to build websocketpp
EXTERNALPROJECT_ADD(
    websocketpp
    PREFIX "${PROJECT_BINARY_DIR}"
    URL "file://${VENDOR_DIRECTORY}/websocketpp-0.8.1.zip"
    URL_MD5 "af0385e19ae5d37b087b1c73b176f10d"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
)

EXTERNALPROJECT_GET_PROPERTY(websocketpp SOURCE_DIR)
SET(WEBSOCKETPP_INCLUDE_DIRS "${SOURCE_DIR}")
