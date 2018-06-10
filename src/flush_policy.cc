#include "flush_policy.h"

#include "timer.h"

void FlushPolicy::SetPolicy(int flush_when_at_least_bytes,
                            int flush_period_secs,
                            int flush_num_entries_each_time) {
  flush_when_at_least_bytes_ = flush_when_at_least_bytes;
  flush_period_secs_ = flush_period_secs;
  flush_num_entries_each_time_ = flush_num_entries_each_time;
  last_flush_timestamp_ = -1;
}

// TODO(zongheng): .
bool FlushPolicy::ShouldFlush() const { return true; }

int FlushPolicy::NumEntriesToFlush() const {
  return flush_num_entries_each_time_;
}

void FlushPolicy::RecordFlush() {
  last_flush_timestamp_ = Timer::NowMicrosecs();
}
