#ifndef QUCS_OBJECT_H
#define QUCS_OBJECT_H

// base object for qucs.
class Object{
 public:
  virtual ~Object(){}

  // clone the object.
  virtual Object* newOne()const {return 0 /*NULL, actually*/;}
};

#endif
