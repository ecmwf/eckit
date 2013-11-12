list( APPEND eckit_mining_srcs
	BisectingKMean.h
	BisectingKMean.cc
SmallestSphere.cc
SmallestSphere.h
Tools.h
)


ecbuild_add_library(TARGET eckit_mining
					INSTALL_HEADERS ALL
					CONDITION	ECKIT_MINING
					SOURCES ${eckit_mining_srcs}
					HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/mining
                                        INCLUDES ${EIGEN3_INCLUDE_DIRS}
					LIBS eckit )

