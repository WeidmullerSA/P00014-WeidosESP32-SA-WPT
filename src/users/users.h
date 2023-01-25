#ifndef users_h
#define users_h

#include <Arduino.h>

class User {  
  public: //Constructor
    User (byte id, String name, String lastName){
        _id = id;
        _name = name;
        _lastName = lastName;
        int points;
        _points = 0;
        _newPoints = 0;
    };
    //Methods
    String getName () { return _name; }
    String getLastName () { return _lastName; }
    byte getID () { return _id; }
    int getPoints () { return _points; }
    void addPoints(int newPoints) { _points += newPoints; }

  private:
    int _points;
    int _newPoints;
    byte _id;
    String _name;
    String _lastName;
};

#endif