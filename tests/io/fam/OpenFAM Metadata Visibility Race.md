# OpenFAM Metadata Visibility Race

## Summary

`test_openfam_metadata_visibility.cc` is a raw OpenFAM reproducer for a
concurrent metadata RPC problem. It does not use eckit's FAM containers. The
test exercises the OpenFAM client and metadata service directly through
`openfam::fam`.

Several independent OpenFAM sessions concurrently allocate the same named
object in an existing region. The expected behavior is:

1. One allocation succeeds.
2. The other allocations return `AlreadyExists`.
3. Each losing session can immediately find the object with `fam_lookup`.

With the current service, the concurrent calls do not complete. The workers
remain blocked until the test watchdogs terminate them.

## Execution Path

The reproducer performs the following operations:

1. The parent creates a uniquely named region.
2. The parent forks eight worker processes.
3. Each worker waits on a pipe so all workers start together.
4. Each worker initializes a fresh OpenFAM session.
5. Each worker looks up the shared region.
6. Each worker calls `fam_allocate` with the same object name.
7. A worker receiving `AlreadyExists` immediately calls `fam_lookup` for that
   object.
8. The parent waits for all workers and then removes the object and region.

The relevant eckit wrapper path is:

```text
FamRegion::ensureObject()
  -> FamRegion::allocateObject()
    -> FamSession::allocateObject()
      -> openfam::fam::fam_allocate()
  -> FamRegion::lookupObject()
    -> FamSession::lookupObject()
      -> openfam::fam::fam_lookup()
```

The test bypasses this wrapper path and calls the OpenFAM API directly so the
behavior can be isolated from eckit object and descriptor handling.

## Reproduction

Build the focused target:

```sh
cmake --build /tmp/bundle/build --target eckit_test_openfam_atomic_descriptor
```

Run it through CTest:

```sh
ctest --test-dir /tmp/bundle/build \
  -R eckit_test_openfam_atomic_descriptor --verbose
```

The configured test environment uses:

```text
FI_PROVIDER=cxi
CXIP_SKIP_AMA_CHECK=true
FI_CXI_LLRING_MODE=never
OPENFAM_INSTALL_DIR=/usr/local/
OPENFAM_ROOT=/usr/local/
ECKIT_FAM_TEST_ENDPOINT=10.42.4.16:8080
```

The test has a 30-second worker and parent watchdog. This is intentionally
shorter than the CTest timeout so a stuck OpenFAM RPC cannot leave child
processes or CTest running indefinitely.

## Observed Result

The failing run reports all eight children terminating with signal 14
(`SIGALRM`) after approximately 30 seconds:

```text
metadata race: child ... terminated by signal 14
...
Test "OpenFAM: object is visible after concurrent allocation" failed
```

No worker reports a normal `AlreadyExists` or lookup error. This indicates
that the calls are blocked inside the OpenFAM client/RPC path rather than
returning a normal metadata error that eckit could handle.

## Findings

The failure occurs below eckit's FAM container layer:

- The test uses `openfam::fam` directly.
- All workers use fresh sessions and the same endpoint.
- The failure is triggered by concurrent allocation of one object name.
- `FamSession::invokeFam()` only translates errors after OpenFAM returns;
  it cannot handle an RPC that never returns.
- `FamRegion::ensureObject()` already handles the expected `AlreadyExists`
  then `lookupObject()` sequence.

The strongest indication is a metadata-service concurrency or RPC lock issue:
concurrent requests for the same object appear to block before OpenFAM can
return `AlreadyExists` or complete the metadata lookup.

The failure is not limited to a newly created object. A separate probe first
created the region and object in a single session, then forked workers that
concurrently called `fam_allocate` for that already-existing object. Those
calls also blocked until the per-worker alarms fired, instead of returning
`AlreadyExists`. This separates the general concurrency failure from the
more specific question of whether newly committed metadata is immediately
visible.

The evidence therefore points to concurrent OpenFAM metadata/RPC handling as
the common trigger:

- A single-session create operation completes.
- Concurrent allocation of a new object hangs.
- Concurrent allocation of an object that already exists also hangs.
- The client receives no OpenFAM error for eckit to translate or retry.

## Recommended Fix

The primary fix should be made in the OpenFAM metadata service and/or its RPC
server:

- Serialize concurrent allocation of the same `(region, object)` name.
- Commit the object metadata before returning `AlreadyExists` to losing
  clients.
- Make a successful allocation immediately visible to `fam_lookup` from a
  different session.
- Do not hold metadata locks while waiting for another RPC or operation that
  may need the same lock.
- Add RPC deadlines or cancellation so a service deadlock becomes a bounded
  client error instead of an indefinite block.

A short client-side retry for `fam_lookup` could handle eventual-consistency
windows if OpenFAM returns a transient not-found error. It will not solve the
current failure by itself because the observed calls block rather than return
such an error. The first priority is making concurrent `fam_allocate` calls
return deterministically for both new and pre-existing object names.

## Scope of This Test

This test is a diagnostic reproducer, not a workaround. Its watchdogs make the
failure deterministic and bounded, while the unique names and cleanup make it
safe to run repeatedly against the configured OpenFAM service.
