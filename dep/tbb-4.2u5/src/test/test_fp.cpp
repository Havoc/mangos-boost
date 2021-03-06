/*
    Copyright 2005-2014 Intel Corporation.  All Rights Reserved.

    This file is part of Threading Building Blocks.

    Threading Building Blocks is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.

    Threading Building Blocks is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Threading Building Blocks; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/

/** This test checks the automatic propagation of master thread FPU settings
    into the worker threads. **/

#include "harness.h"
#define private public
#include "tbb/task.h"
#undef private
#include "tbb/parallel_for.h"
#include "tbb/task_scheduler_init.h"

const int N = 500000;

#if ( __TBB_x86_32 || __TBB_x86_64 ) && __TBB_CPU_CTL_ENV_PRESENT && !defined(__TBB_WIN32_USE_CL_BUILTINS)

const int FE_TONEAREST = 0x0000,
          FE_DOWNWARD = 0x0400,
          FE_UPWARD = 0x0800,
          FE_TOWARDZERO = 0x0c00,
          FE_RND_MODE_MASK = FE_TOWARDZERO,
          SSE_RND_MODE_MASK = FE_RND_MODE_MASK << 3,
          SSE_DAZ = 0x0040,
          SSE_FTZ = 0x8000,
          SSE_MODE_MASK = SSE_DAZ | SSE_FTZ,
          SSE_STATUS_MASK = 0x3F;

const int NumSseModes = 4;
const int SseModes[NumSseModes] = { 0, SSE_DAZ, SSE_FTZ, SSE_DAZ | SSE_FTZ };

#if _WIN64 && !__TBB_X86_MSVC_INLINE_ASM_AVAILABLE && !__MINGW64__
// MinGW uses inline implementation from tbb/machine/linux_intel64.h

#include <float.h>

inline void __TBB_get_cpu_ctl_env ( tbb::internal::cpu_ctl_env* fe ) {
    fe->x87cw = short(_control87(0, 0) & _MCW_RC) << 2;
    fe->mxcsr = _mm_getcsr();
}
inline void __TBB_set_cpu_ctl_env ( const tbb::internal::cpu_ctl_env* fe ) {
    ASSERT( (fe->x87cw & FE_RND_MODE_MASK) == ((fe->x87cw & FE_RND_MODE_MASK) >> 2 & _MCW_RC) << 2, "Check float.h constants" );
    _control87( (fe->x87cw & FE_RND_MODE_MASK) >> 6, _MCW_RC );
    _mm_setcsr( fe->mxcsr );
}

#endif /*  _WIN64 && !__TBB_X86_MSVC_INLINE_ASM_AVAILABLE && !__MINGW64__ */

inline int GetRoundingMode ( bool checkConsistency = true ) {
    tbb::internal::cpu_ctl_env ctl;
    ctl.get_env();
    ASSERT( !checkConsistency || (ctl.mxcsr & SSE_RND_MODE_MASK) >> 3 == (ctl.x87cw & FE_RND_MODE_MASK), NULL );
    return ctl.x87cw & FE_RND_MODE_MASK;
}

inline void SetRoundingMode ( int mode ) {
    tbb::internal::cpu_ctl_env ctl;
    ctl.get_env();
    ctl.mxcsr = (ctl.mxcsr & ~SSE_RND_MODE_MASK) | (mode & FE_RND_MODE_MASK) << 3;
    ctl.x87cw = short((ctl.x87cw & ~FE_RND_MODE_MASK) | (mode & FE_RND_MODE_MASK));
    ctl.set_env();
}

inline int GetSseMode () {
    tbb::internal::cpu_ctl_env ctl;
    ctl.get_env();
    return ctl.mxcsr & SSE_MODE_MASK;
}

inline void SetSseMode ( int mode ) {
    tbb::internal::cpu_ctl_env ctl;
    ctl.get_env();
    ctl.mxcsr = (ctl.mxcsr & ~SSE_MODE_MASK) | (mode & SSE_MODE_MASK);
    ctl.set_env();
}

#include "harness_barrier.h"

class CheckNoSseStatusPropagationBody : public NoAssign {
    Harness::SpinBarrier &barrier;
public:
    CheckNoSseStatusPropagationBody( Harness::SpinBarrier &_barrier ) : barrier(_barrier) {}
    void operator()( const tbb::blocked_range<int>& ) const {
        barrier.wait();
        tbb::internal::cpu_ctl_env ctl;
        ctl.get_env();
        ASSERT( (ctl.mxcsr & SSE_STATUS_MASK) == 0, "FPU control status bits have been propagated." );
    }
};

void CheckNoSseStatusPropagation() {
    tbb::internal::cpu_ctl_env ctl;
    ctl.get_env();
    ctl.mxcsr |= SSE_STATUS_MASK;
    ctl.set_env();
    const int num_threads = tbb::task_scheduler_init::default_num_threads();
    Harness::SpinBarrier barrier(num_threads);
    tbb::task_scheduler_init init(num_threads);
    tbb::parallel_for( tbb::blocked_range<int>(0, num_threads), CheckNoSseStatusPropagationBody(barrier) );
    ctl.mxcsr &= ~SSE_STATUS_MASK;
    ctl.set_env();
}

#elif defined(_M_ARM) || defined(__TBB_WIN32_USE_CL_BUILTINS)
const int NumSseModes = 1;
const int SseModes[NumSseModes] = { 0 };

inline int GetSseMode () { return 0; }
inline void SetSseMode ( int ) {}

const int FE_TONEAREST = _RC_NEAR,
          FE_DOWNWARD = _RC_DOWN,
          FE_UPWARD = _RC_UP,
          FE_TOWARDZERO = _RC_CHOP;

inline int GetRoundingMode ( bool = true ) {
    tbb::internal::cpu_ctl_env ctl;
    ctl.get_env();
    return ctl.my_ctl;
}
inline void SetRoundingMode ( int mode ) {
    tbb::internal::cpu_ctl_env ctl;
    ctl.my_ctl = mode;
    ctl.set_env();
}

void CheckNoSseStatusPropagation() {}
#else /* Other archs */

#include <fenv.h>

const int RND_MODE_MASK = FE_TONEAREST | FE_DOWNWARD | FE_UPWARD | FE_TOWARDZERO;

const int NumSseModes = 1;
const int SseModes[NumSseModes] = { 0 };

inline int GetRoundingMode ( bool = true ) { return fegetround(); }
inline void SetRoundingMode ( int rnd ) { fesetround(rnd); }

inline int GetSseMode () { return 0; }
inline void SetSseMode ( int ) {}

void CheckNoSseStatusPropagation() {}
#endif /* Other archs */

const int NumRoundingModes = 4;
const int RoundingModes[NumRoundingModes] = { FE_TONEAREST, FE_DOWNWARD, FE_UPWARD, FE_TOWARDZERO };

class RoundingModeCheckBody {
    int m_mode;
    int m_sseMode;
public:
    void operator() ( int /*iter*/ ) const {
        ASSERT( GetRoundingMode() == m_mode, "FPU control state has not been propagated." );
        ASSERT( GetSseMode() == m_sseMode, "SSE control state has not been propagated." );
    }

    RoundingModeCheckBody ( int mode, int sseMode ) : m_mode(mode), m_sseMode(sseMode) {}
};

void TestArenaFpuEnvPropagation( int id ) {
    // TBB scheduler instance in a master thread captures the FPU control state
    // at the moment of its initialization and passes it to the workers toiling
    // on its behalf.
    for( int k = 0; k < NumSseModes; ++k ) {
        int sse_mode = SseModes[(k + id) % NumSseModes];
        SetSseMode( sse_mode );
        for( int i = 0; i < NumRoundingModes; ++i ) {
            int mode = RoundingModes[(i + id) % NumRoundingModes];
            SetRoundingMode( mode );
            // New mode must be set before TBB scheduler is initialized
            tbb::task_scheduler_init init;
            tbb::parallel_for( 0, N, 1, RoundingModeCheckBody(mode, sse_mode) );
            ASSERT( GetRoundingMode() == mode, NULL );
        }
    }
}

#if __TBB_FP_CONTEXT
void TestArenaFpuEnvPersistence( int id ) {
    // Since the following loop uses auto-initialization, the scheduler instance
    // implicitly created by the first parallel_for invocation will persist
    // until the thread ends, and thus workers will use the mode set by the
    // first iteration.
    int captured_mode = RoundingModes[id % NumRoundingModes];
    int captured_sse_mode = SseModes[id % NumSseModes];
    for( int k = 0; k < NumSseModes; ++k ) {
        int sse_mode = SseModes[(k + id) % NumSseModes];
        SetSseMode( sse_mode );
        for( int i = 0; i < NumRoundingModes; ++i ) {
            int mode = RoundingModes[(i + id) % NumRoundingModes];
            SetRoundingMode( mode );
            tbb::parallel_for( 0, N, 1, RoundingModeCheckBody(captured_mode, captured_sse_mode) );
            ASSERT( GetRoundingMode() == mode, NULL );
        }
    }
}
#endif

class LauncherBody {
public:
    void operator() ( int id ) const {
        TestArenaFpuEnvPropagation( id );
#if __TBB_FP_CONTEXT
        TestArenaFpuEnvPersistence( id );
#endif
    }
};

void TestFpuEnvPropagation () {
    const int p = tbb::task_scheduler_init::default_num_threads();
    // The test should be run in an oversubscription mode. So create 4*p threads but
    // limit the oversubscription for big machines (p>32) with 4*32+(p-32) threads.
    const int num_threads = p + (NumRoundingModes-1)*min(p,32);
    NativeParallelFor ( num_threads, LauncherBody() );
}

void TestCpuCtlEnvApi () {
    for( int k = 0; k < NumSseModes; ++k ) {
        SetSseMode( SseModes[k] );
        for( int i = 0; i < NumRoundingModes; ++i ) {
            SetRoundingMode( RoundingModes[i] );
            ASSERT( GetRoundingMode() == RoundingModes[i], NULL );
            ASSERT( GetSseMode() == SseModes[k], NULL );
        }
    }
}

#if __TBB_FP_CONTEXT
const int numModes = NumRoundingModes*NumSseModes;
const int numArenas = 4;
tbb::task_group_context *contexts[numModes];
// +1 for a default context
int roundingModes[numModes+numArenas];
int sseModes[numModes+numArenas];

class TestContextFpuEnvBody {
    int arenaNum;
    int mode;
    int depth;
public:
    TestContextFpuEnvBody( int _arenaNum, int _mode, int _depth = 0 ) : arenaNum(_arenaNum), mode(_mode), depth(_depth) {}
    void operator()( const tbb::blocked_range<int> &r ) const;
};

inline void SetMode( int mode ) {
    SetRoundingMode( roundingModes[mode] );
    SetSseMode( sseModes[mode] );
}

inline void AssertMode( int mode ) {
    ASSERT( GetRoundingMode() == roundingModes[mode], "FPU control state has not been set correctly." );
    ASSERT( GetSseMode() == sseModes[mode], "SSE control state has not been set correctly." );
}

inline int SetNextMode( int mode, int step ) {
    const int nextMode = (mode+step)%numModes;
    SetMode( nextMode );
    return nextMode;
}

class TestContextFpuEnvTask : public tbb::task {
    int arenaNum;
    int mode;
    int depth;
#if __TBB_CPU_CTL_ENV_PRESENT
    static const int MAX_DEPTH = 3;
#else
    static const int MAX_DEPTH = 4;
#endif
public:
    TestContextFpuEnvTask( int _arenaNum, int _mode, int _depth = 0 ) : arenaNum(_arenaNum), mode(_mode), depth(_depth) {}
    tbb::task* execute() {
        AssertMode( mode );
        if ( depth < MAX_DEPTH ) {
            // Test default context.
            const int newMode1 = SetNextMode( mode, depth+1 );
            tbb::parallel_for( tbb::blocked_range<int>(0, numModes+1), TestContextFpuEnvBody( arenaNum, mode, depth+1 ) );
            AssertMode( newMode1 );

            // Test user default context.
            const int newMode2 = SetNextMode( newMode1, depth+1 );
            tbb::task_group_context ctx1;
            const int newMode3 = SetNextMode( newMode2, depth+1 );
            tbb::parallel_for( tbb::blocked_range<int>(0, numModes+1), TestContextFpuEnvBody( arenaNum, mode, depth+1 ), ctx1 );
            AssertMode( newMode3 );

            // Test user context which captured FPU control settings.
            const int newMode4 = SetNextMode( newMode3, depth+1 );
            // Capture newMode4
            ctx1.capture_fp_settings();
            const int newMode5 = SetNextMode( newMode4, depth+1 );
            tbb::parallel_for( tbb::blocked_range<int>(0, numModes+1), TestContextFpuEnvBody( arenaNum, newMode4, depth+1 ), ctx1 );
            AssertMode( newMode5 );

            // And again test user context which captured FPU control settings to check multiple captures.
            const int newMode6 = SetNextMode( newMode5, depth+1 );
            // Capture newMode6
            ctx1.capture_fp_settings();
            const int newMode7 = SetNextMode( newMode6, depth+1 );
            tbb::parallel_for( tbb::blocked_range<int>(0, numModes+1), TestContextFpuEnvBody( arenaNum, newMode6, depth+1 ), ctx1 );
            AssertMode( newMode7 );

            // Test an isolated context. The isolated context should use default FPU control settings.
            const int newMode8 = SetNextMode( newMode7, depth+1 );
            tbb::task_group_context ctx2( tbb::task_group_context::isolated );
            const int newMode9 = SetNextMode( newMode8, depth+1 );
            tbb::parallel_for( tbb::blocked_range<int>(0, numModes+1), TestContextFpuEnvBody( arenaNum, numModes+arenaNum, depth+1 ), ctx2 );
            AssertMode( newMode9 );

            // The binding should not owerrite captured FPU control settings.
            const int newMode10 = SetNextMode( newMode9, depth+1 );
            tbb::task_group_context ctx3;
            ctx3.capture_fp_settings();
            const int newMode11 = SetNextMode( newMode10, depth+1 );
            tbb::parallel_for( tbb::blocked_range<int>(0, numModes+1), TestContextFpuEnvBody( arenaNum, newMode10, depth+1 ), ctx3 );
            AssertMode( newMode11 );

            // Restore initial mode since user code in tbb::task::execute should not change FPU settings.
            SetMode( mode );
        }

        return NULL;
    }
};

void TestContextFpuEnvBody::operator()( const tbb::blocked_range<int> &r ) const {
    AssertMode( mode );

    const int newMode = SetNextMode( mode, depth+2 );

    int end = r.end();
    if ( end-1 == numModes ) {
        // For a default context our mode should be inherited.
        tbb::task::spawn_root_and_wait( 
            *new( tbb::task::allocate_root() ) TestContextFpuEnvTask( arenaNum, mode, depth ) );
        AssertMode( newMode );
        end--;
    }
    for ( int i=r.begin(); i<end; ++i ) {
        tbb::task::spawn_root_and_wait( 
            *new( tbb::task::allocate_root(*contexts[i]) ) TestContextFpuEnvTask( arenaNum, i, depth ) );
        AssertMode( newMode );
    }

    // Restore initial mode since user code in tbb::task::execute should not change FPU settings.
    SetMode( mode );
}

class TestContextFpuEnvNativeLoopBody {
public:
    void operator() ( int arenaNum ) const {
        SetMode(numModes+arenaNum);
        tbb::task_scheduler_init init;
        tbb::task::spawn_root_and_wait( *new (tbb::task::allocate_root()) TestContextFpuEnvTask( arenaNum, numModes+arenaNum ) );
    }
};

#if TBB_USE_EXCEPTIONS
const int NUM_ITERS = 1000;
class TestContextFpuEnvEhBody {
    int mode;
    int eh_iter;
    int depth;
public:
    TestContextFpuEnvEhBody( int _mode, int _eh_iter, int _depth = 0 ) : mode(_mode), eh_iter(_eh_iter), depth(_depth) {}
    void operator()( const tbb::blocked_range<int> &r ) const {
        AssertMode( mode );
        if ( depth < 1 ) {
            const int newMode1 = SetNextMode( mode, 1 );
            tbb::task_group_context ctx;
            ctx.capture_fp_settings();
            const int newMode2 = SetNextMode( newMode1, 1 );
            try {
                tbb::parallel_for( tbb::blocked_range<int>(0, NUM_ITERS), TestContextFpuEnvEhBody(newMode1,rand()%NUM_ITERS,1), tbb::simple_partitioner(), ctx );
            } catch (...) {
                AssertMode( newMode2 );
                if ( r.begin() == eh_iter ) throw;
            }
            AssertMode( newMode2 );
            SetMode( mode );
        } else if ( r.begin() == eh_iter ) throw 0;
    }
};

class TestContextFpuEnvEhNativeLoopBody {
public:
    void operator() ( int arenaNum ) const {
        SetMode( arenaNum%numModes );
        try {
            tbb::parallel_for( tbb::blocked_range<int>(0, NUM_ITERS), TestContextFpuEnvEhBody((arenaNum+1)%numModes,rand()%NUM_ITERS),
                tbb::simple_partitioner(), *contexts[(arenaNum+1)%numModes] );
            ASSERT( false, "parallel_for has not thrown an exception." );
        } catch (...) {
            AssertMode( arenaNum%numModes );
        }
    }
};
#endif /* TBB_USE_EXCEPTIONS */

void TestContextFpuEnv() {
    // Prepare contexts' fp modes.
    for ( int i = 0, modeNum = 0; i < NumRoundingModes; ++i ) {
        const int roundingMode = RoundingModes[i];
        SetRoundingMode( roundingMode );
        for( int j = 0; j < NumSseModes; ++j, ++modeNum ) {
            const int sseMode = SseModes[j];
            SetSseMode( sseMode );

            contexts[modeNum] = new tbb::task_group_context( tbb::task_group_context::isolated,
                tbb::task_group_context::default_traits | tbb::task_group_context::fp_settings );
            roundingModes[modeNum] = roundingMode;
            sseModes[modeNum] = sseMode;
        }
    }
    // Prepare arenas' fp modes.
    for ( int arenaNum = 0; arenaNum < numArenas; ++arenaNum ) {
        roundingModes[numModes+arenaNum] = roundingModes[arenaNum%numModes];
        sseModes[numModes+arenaNum] = sseModes[arenaNum%numModes];
    }
    NativeParallelFor( numArenas, TestContextFpuEnvNativeLoopBody() );
#if TBB_USE_EXCEPTIONS
    NativeParallelFor( numArenas, TestContextFpuEnvEhNativeLoopBody() );
#endif
    for ( int modeNum = 0; modeNum < numModes; ++modeNum )
        delete contexts[modeNum];
}
#endif /* __TBB_FP_CONTEXT */

int TestMain () {
    TestCpuCtlEnvApi();
    TestFpuEnvPropagation();
    CheckNoSseStatusPropagation();
#if __TBB_FP_CONTEXT
    TestContextFpuEnv();
#endif
    return Harness::Done;
}
