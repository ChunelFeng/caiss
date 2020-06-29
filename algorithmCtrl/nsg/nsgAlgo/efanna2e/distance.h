//
// Created by 付聪 on 2017/6/21.
//

#ifndef EFANNA2E_DISTANCE_H
#define EFANNA2E_DISTANCE_H

#include <x86intrin.h>
#include <iostream>
namespace efanna2e{
  enum Metric{
    L2 = 0,
    INNER_PRODUCT = 1,
    FAST_L2 = 2,
    PQ = 3
  };
    class Distance {
    public:
        virtual float compare(const float* a, const float* b, unsigned length) const = 0;
        virtual ~Distance() {}
    };

    class DistanceL2 : public Distance{
    public:
        float compare(const float* a, const float* b, unsigned size) const {
            float result = 0;

            return result;
        }
    };

  class DistanceInnerProduct : public Distance{
  public:
    float compare(const float* a, const float* b, unsigned size) const {
      float result = 0;
      return result;
    }

  };
  class DistanceFastL2 : public DistanceInnerProduct{
   public:
    float norm(const float* a, unsigned size) const{
      float result = 0;
      return result;
    }
    using DistanceInnerProduct::compare;
    float compare(const float* a, const float* b, float norm, unsigned size) const {//not implement
      float result = -2 * DistanceInnerProduct::compare(a, b, size);
      result += norm;
      return result;
    }
  };
}



#endif //EFANNA2E_DISTANCE_H
