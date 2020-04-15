

/*
  _lastMomentum = _momentum;
  _lastPosition = _position;
  _lastTime = fe_engine::getInstance()->time();


#ifdef __INTEGRATION_EULERSTEP__
  // Euler Step Implementation

  _calcForce( _lastTime, _force );

  _momentum += timeStep * _force;

  _position += constants::c * velocity() * timeStep;

#endif

#ifdef __INTEGRATION_RUNGEKUTTA4__
  // RK4 Implementation

 
  // fourVector xk_1 = _position;
  threeVector pk_1 = _momentum;
  threeVector fk_1( 0.0 );
  _calcForce( _lastTime, fk_1 );


  _position = _lastPosition + 0.5 * timeStep * velocity() * constants::c;
  threeVector pk_2 = _lastMomentum + 0.5 * timeStep * fk_1;
  _momentum = pk_2;
  threeVector fk_2 ( 0.0 );
  _calcForce( _lastTime + 0.5 * timeStep, fk_2 );


  _position = _lastPosition + 0.5 * timeStep * velocity() * constants::c;
  threeVector pk_3 = _lastMomentum + 0.5 * timeStep * fk_2;
  _momentum = pk_3;
  threeVector fk_3( 0.0 );
  _calcForce( _lastTime + 0.5 * timeStep, fk_3 );


  _position = _lastPosition + timeStep * velocity() * constants::c;
  threeVector pk_4 = _lastMomentum + timeStep * fk_3;
  _momentum = pk_4;
  threeVector fk_4( 0.0 );
  _calcForce( _lastTime + timeStep, fk_4 );


  _position = _lastPosition + timeStep * ( pk_1 + 2.0*pk_2 + 2.0*pk_3 + pk_4 ) * 0.1666666666666666666666666666666666 * _invMass * constants::c * ( 1.0 / gamma() );
  _momentum = _lastMomentum + timeStep * ( fk_1 + 2.0*fk_2 + 2.0*fk_3 + fk_4 ) * 0.1666666666666666666666666666666666;

  _force = fk_4;

#endif

*/
