list( APPEND eckit_stats_srcs
    Comparator.cc
    Comparator.h
    Distribution.cc
    Distribution.h
    Field.cc
    Field.h
    FieldStatistics.cc
    FieldStatistics.h
    Method.cc
    Method.h
    Statistics.cc
    Statistics.h
    comparator/ComparatorT.cc
    comparator/ComparatorT.h
    comparator/MissingValues.cc
    comparator/MissingValues.h
    comparator/Spectral.cc
    comparator/Spectral.h
    detail/AngleT.cc
    detail/AngleT.h
    detail/CentralMomentsT.cc
    detail/CentralMomentsT.h
    detail/Counter.cc
    detail/Counter.h
    detail/CounterBinary.cc
    detail/CounterBinary.h
    detail/ModeT.cc
    detail/ModeT.h
    detail/PNormsT.h
    detail/ScalarT.h
    distribution/DistributionT.cc
    distribution/DistributionT.h
    field/CentralMomentStats.cc
    field/CentralMomentStats.h
    field/CounterStats.cc
    field/CounterStats.h
    field/ModeStats.cc
    field/ModeStats.h
    method/MethodT.cc
    method/MethodT.h
    statistics/SimplePackingEntropy.cc
    statistics/SimplePackingEntropy.h
    statistics/Spectral.cc
    statistics/Spectral.h
    statistics/StatisticsT.cc
    statistics/StatisticsT.h
    util/mutex.h
)

ecbuild_add_library(
    TARGET             eckit_stats
    TYPE               SHARED
    INSTALL_HEADERS    LISTED
    HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/stats
    SOURCES            ${eckit_stats_srcs}
    PUBLIC_LIBS        eckit )

