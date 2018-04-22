
#!/bin/bash
set -x

# Defaults to 1, or 1st arg if provided.
# Master is assigned port 6369.  Chain node i gets port 6369+i.
NUM_NODES=${1:-1}

gcs_normal=0
gcs_ckptonly=1
gcs_ckptflush=2
gcs_mode=${gcs_ckptflush}
gcs_mode=${gcs_normal}

function setup() {
    # pushd build
    # make -j
    # popd

    # Master.
    # ./redis/src/redis-server --loadmodule ./build/src/libmaster.so --port 6369 &> master.log &

    # Assume by default the master & 1 node are already running, at 6369, 6370, respectively.
    port=6370
    while true; do
        result=$(pgrep -a redis-server | grep $port)
        if [ -z "${result}" ]; then
            echo 'breaking...'
            break
        fi
        port=$(expr $port + 1)
    done

    echo 'break!'

    for i in $(seq 1 $NUM_NODES); do
      #LD_PRELOAD=/usr/lib/libprofiler.so CPUPROFILE=/tmp/pprof-${i} \
      #  ./redis/src/redis-server --loadmodule ./build/src/libmember.so \
      #  --protected-mode no --port $port &> $port.log &

      # sudo cgexec -g memory:redisgroup ./redis/src/redis-server --loadmodule ./build/src/libmember.so ${gcs_mode} --port $port --protected-mode no &> $port.log &
      ./redis/src/redis-server --loadmodule ./build/src/libmember.so ${gcs_mode} --port $port --protected-mode no &> $port.log &

      sleep 0.5
      ./redis/src/redis-cli -p 6369 MASTER.ADD 127.0.0.1 $port

      # Have chain nodes connect to master.
      sleep 0.5
      ./redis/src/redis-cli -p $port MEMBER.CONNECT_TO_MASTER 127.0.0.1 6369

      port=$(expr $port + 1)
    done
}

setup
