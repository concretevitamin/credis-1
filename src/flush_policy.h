#ifndef CREDIS_FLUSH_POLICY_
#define CREDIS_FLUSH_POLICY_

// A simple flush policy.  Constant flush rate, non-adaptive.
class FlushPolicy {
 public:
  // Default values.  A flush rate of 30MB/minute, assuming ~0.5KB/entry (e.g.,
  // task table).
  FlushPolicy()
      : flush_when_at_least_bytes_(0),
        flush_period_secs_(10),
        flush_num_entries_each_time_(10000),
        last_flush_timestamp_(-1) {}

  void SetPolicy(int flush_when_at_least_bytes, int flush_period_secs,
                 int flush_num_entries_each_time);

  // Flush when two conditions are met:
  // (1) Current memory usage of this process is >=
  // "flush_when_at_least_bytes_".
  // (2) Current time > "flush_period_secs_" + last processed flush's ending
  // timestamp.
  bool ShouldFlush() const;

  // A constant, "flush_num_entries_each_time_".  Serves as an upper bound.
  int NumEntriesToFlush() const;

  // Must be called after each processed flush (even with 0 entries).
  void RecordFlush();

 private:
  int flush_when_at_least_bytes_;
  int flush_period_secs_;
  int flush_num_entries_each_time_;

  // Microsecs.  -1 indicates no flush has been processed yet.
  double last_flush_timestamp_;
};

#endif  // CREDIS_FLUSH_POLICY_
