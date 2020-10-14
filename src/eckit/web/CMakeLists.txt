list( APPEND eckit_web_srcs
AgentResource.cc
AgentResource.h
CgiResource.cc
CgiResource.h
Configure.cc
FileResource.cc
FileResource.h
FtpRequest.h
Html.cc
Html.h
HtmlObject.cc
HtmlObject.h
HtmlResource.cc
HtmlResource.h
HttpResource.cc
HttpResource.h
HttpStream.cc
HttpStream.h
HttpServer.cc
HttpServer.h
HttpService.cc
HttpService.h
HttpUser.h
JSONResource.cc
JSONResource.h
JavaAgent.cc
JavaAgent.h
JavaResource.cc
JavaResource.h
JavaServer.cc
JavaServer.h
JavaService.cc
JavaService.h
JavaUser.cc
JavaUser.h
Url.cc
Url.h)


ecbuild_add_library( TARGET eckit_web TYPE SHARED
					 INSTALL_HEADERS LISTED
					 SOURCES
						${eckit_web_srcs}
					 PERSISTENT
						HtmlObject.h
					 HEADER_DESTINATION
						${INSTALL_INCLUDE_DIR}/eckit/web
					 PUBLIC_LIBS
						eckit )
