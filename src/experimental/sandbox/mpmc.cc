// #include "eckit/container/Queue.h"

int main()
{
  #if 0
  Queue<int> q(8);


  std::vector<std::thread> producers;
  for (int id = 0 ; id < nProducers ; ++id) {
    producers.emplace_back(std::thread([&q, id]{
              for (int j = 0; j < nbToProduce; ++j) {
                std::ostringstream tmp; tmp << id << " --> " << 10*id+j; print(tmp.str());
                q.push(10*id+j);
              }
        }));
  }


  std::vector<std::thread> consumers;
  for (int id = 0 ; id < nConsumers ; ++id) {
        consumers.emplace_back(std::thread([&q, id]{
              for (int j = 0; j < nbToConsume; ++j) {
                auto e = q.pop();
                std::ostringstream tmp; tmp << id << "    <-- " << e; print(tmp.str());
              }
        }));
  }

  for(auto& p : producers) {
    p.join();
  }

  for(auto& c : consumers) {
    c.join();
  }
  #endif
}