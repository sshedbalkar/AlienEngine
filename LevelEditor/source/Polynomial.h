#ifndef __Polynomial__
#define __Polynomial__

#include <math.h>
#include <stdio.h>
namespace Framework
{

// Polynomials
class Polynomial
{
protected:
  int n;
  double coef[16];
public:
  static Polynomial NullPolynomial;
    Polynomial ();
   ~Polynomial ()
  {
  }

  Polynomial (const double &a6, const double &a5, const double &a4,
              const double &a3, const double &a2, const double &a1,
              const double &a0)
  {
    coef[0] = a0;
    coef[1] = a1;
    coef[2] = a2;
    coef[3] = a3;
    coef[4] = a4;
    coef[5] = a5;
    coef[6] = a6;
    n = 6;
  }
  Polynomial (const double &a, const double &b, const double &c,
              const double &d, const double &e)
  {
    coef[0] = e;
    coef[1] = d;
    coef[2] = c;
    coef[3] = b;
    coef[4] = a;
    n = 4;
  }
  Polynomial (const double &a, const double &b, const double &c,
              const double &d)
  {
    coef[0] = d;
    coef[1] = c;
    coef[2] = b;
    coef[3] = a;
    n = 3;
  }
  Polynomial (const double &a, const double &b, const double &c)
  {
    coef[0] = c;
    coef[1] = b;
    coef[2] = a;
    n = 2;
  }
  Polynomial (const double &a, const double &b)
  {
    coef[0] = b;
    coef[1] = a;
    n = 1;
  }
  Polynomial (const double &a)
  {
    coef[0] = a;
    n = 0;
  }

  // Access Polynomial class components
  double &operator[] (int i)
  {
    return coef[i];
  }

  const double operator[] (int i) const
  {
    return coef[i];
  }

  // Unary operators
  Polynomial operator + () const
  {
    return *this;
  }
  Polynomial operator - () const;

  Polynomial & operator += (const Polynomial &);
  Polynomial & operator -= (const Polynomial &);
  Polynomial & operator *= (const Polynomial &);
  Polynomial & operator *= (const double &);
  Polynomial & operator /= (const double &);

  // Binary operators
  friend Polynomial operator + (const Polynomial &, const Polynomial &);
  friend Polynomial operator - (const Polynomial &, const Polynomial &);

  friend Polynomial operator *(const Polynomial &, const Polynomial &);

  friend Polynomial operator *(const Polynomial &, const double &);
  friend Polynomial operator *(const double &, const Polynomial &);
  friend Polynomial operator / (const Polynomial &, const double &);

  // Evaluates polynomial
  double Eval (const double &);

  // Solve
  int SolveQuadratic (double &, double &);
  int SolveQuadratic (double *);
  int SolveCubic (double *);
  int SolveQuartic (double *);
  int Solve (double *);

  int Degree ()
  {
    return n;
  }
  void Check ();
};
}
#endif