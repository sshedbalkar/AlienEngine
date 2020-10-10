#include "Polynomial.h"

namespace Framework
{

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

#undef EPSILON
#define EPSILON 1.0e-10
#define COEFF_LIMIT 1.0e-16

#define FUDGE_FACTOR1 1.0e12
#define FUDGE_FACTOR2 -1.0e-5
#define FUDGE_FACTOR3 1.0e-7

#define TWO_PI 6.283185207179586476925286766560
#define TWO_PI_3  2.0943951023931954923084
#define TWO_PI_43 4.1887902047863909846168
#define MAX_ITERATIONS 50
#define MAXPOW 32

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

Polynomial::Polynomial ()
{
  n = 0;
  coef[0] = 0.0;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/*!
  \brief Set the null polynomial member constant.
*/
Polynomial Polynomial::NullPolynomial (0.0);

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/*!
  \brief Evaluates the polynomial, this function is optimized (computations are
  performed inline) for low degree (cubic and lower) polynomials.
*/
double
Polynomial::Eval (const double &x)
{
  if (n == -1)
    return 0.0;
  else if (n == 0)
    return (coef[0]);
  else if (n == 1)
    return (coef[0] + x * coef[1]);
  else if (n == 2)
    return (coef[0] + x * (coef[1] + x * coef[2]));
  else if (n == 3)
    return (coef[0] + x * (coef[1] + x * (coef[2] + x * coef[3])));
  else
    {
      double y = coef[n];

      for (int i = n - 1; i >= 0; i--)
        {
          y *= x;
          y += coef[i];
        }
      return y;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/*!
  \brief Check the degree of a polynomial, decreading the degree if
  higher coefficients are null.
*/
void
Polynomial::Check ()
{
  while (n > 0)
    {
      if (coef[n] != 0.0)
        break;
      n--;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/*!
  \brief Destructive sum of two polynomials.
*/
Polynomial & Polynomial::operator += (const Polynomial & u)
{
  if (u.n > n)
    {
      for (int i = 0; i <= n; i++)
        {
          coef[i] += u[i];
        }
      for (int i = n + 1; i <= u.n; i++)
        {
          coef[i] = u[i];
        }
      n = u.n;
    }
  else
    {
      for (int i = 0; i <= u.n; i++)
        {
          coef[i] += u[i];
        }
    }

  return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/*!
  \brief Destructive difference of two polynomials.
*/
Polynomial & Polynomial::operator -= (const Polynomial & u)
{
  if (u.n > n)
    {
      for (int i = 0; i <= n; i++)
        {
          coef[i] -= u[i];
        }
      for (int i = n + 1; i <= u.n; i++)
        {
          coef[i] = -u[i];
        }
      n = u.n;
    }
  else
    {
      for (int i = 0; i <= u.n; i++)
        {
          coef[i] -= u[i];
        }
      /*
         for (i=n;i>-1;i--)
         {
         if (coef[i]!=0.0)
         break;
         }
         n=i;
       */
    }

  return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/*!
  \brief Scale a polynomial by a real value. If x=0, just set
  polynomial to null, if x=1, return the polynomial unchanged,
  otherwize compute.
*/
Polynomial & Polynomial::operator *= (const double &e)
{
  if (e == 0.0)
    {
      n = -1;
    }
  else if (e == 1.0)
    {
      return *this;
    }
  else
    {
      for (int i = 0; i <= n; i++)
        {
          coef[i] *= e;
        }
    }
  return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

Polynomial & Polynomial::operator /= (const double &e)
{
  if (e == 1.0)
    {
      return *this;
    }

  for (int i = 0; i <= n; i++)
    {
      coef[i] /= e;
    }
  return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/*!
  \brief Multiply a polynomial by a scalar value. Optimizations are provided
  if scalar is 0.0 or 1.0.
*/
Polynomial operator *(const Polynomial & u, const double &e)
{
  // Identity
  if (e == 1.0)
    {
      return u;
    }
  // Null polynomial
  else if (e == 0.0)
    {
      return Polynomial::NullPolynomial;
    }
  else
    {
      Polynomial r = u;
      r *= e;
      return r;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

// Binary operators
Polynomial operator + (const Polynomial & u, const Polynomial & v)
{
  Polynomial r = u;
  r += v;

  return r;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

Polynomial operator - (const Polynomial & u, const Polynomial & v)
{
  Polynomial r = u;
  r -= v;

  return r;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

Polynomial & Polynomial::operator *= (const Polynomial & u)
{
  Polynomial r = (*this) * u;
  *this = r;
  return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

Polynomial operator *(const Polynomial & u, const Polynomial & v)
{
  Polynomial r;

  r.n = u.n + v.n;
  for (int i = 0; i <= r.n; i++)
    r.coef[i] = 0.0;

  for (int i = 0; i <= u.n; i++)
    for (int j = 0; j <= v.n; j++)
      r[i + j] += u[i] * v[j];
  return r;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/*!
  \brief Unary.
*/
inline Polynomial Polynomial::operator - ()
     const
     {
       Polynomial p;
       p.n = n;
       for (int i = 0; i <= n; i++)
         {
           p[i] = -coef[i];
         }
       return
         p;
     }

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

Polynomial operator *(const double &a, const Polynomial & p)
{
  return p * a;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

Polynomial operator / (const Polynomial & p, const double &a)
{
  return p * (1.0 / a);
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/*!
  \brief Solve quadratic equations. This function store the 
  sorted roots in an array and returns the number of roots.
*/
int
Polynomial::SolveQuadratic (double *y)
{
  double d, t, a, b, c;
  a = coef[2];
  b = -coef[1];
  c = coef[0];
  if (a == 0.0)
    {
      if (b == 0.0)
        return 0;
      y[0] = y[1] = c / b;
      return 1;
    }
  d = b * b - 4.0 * a * c;
  if (d < 0.0)
    return 0;
  else if (fabs (d) < COEFF_LIMIT)
    {
      y[0] = y[1] = 0.5 * b / a;
      return 1;
    }
  d = sqrt (d);
  t = 0.5 / a;
  if (t > 0.0)
    {
      y[0] = (b - d) * t;
      y[1] = (b + d) * t;
    }
  else
    {
      y[0] = (b + d) * t;
      y[1] = (b - d) * t;
    }
  return 2;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/*!
  \brief Solve cubic equations. 
*/
int
Polynomial::SolveCubic (double *y)
{
  double Q, R, Q3, R2, sQ, d, an, theta;
  double A2, a1, a2, a3;

  double a0 = coef[3];

  if (fabs (a0) < EPSILON)
    {
      return SolveQuadratic (y);
    }
  else
    {
      if (a0 != 1.0)
        {
          a1 = coef[2] / a0;
          a2 = coef[1] / a0;
          a3 = coef[0] / a0;
        }
      else
        {
          a1 = coef[2];
          a2 = coef[1];
          a3 = coef[0];
        }
    }

  A2 = a1 * a1;

  Q = (A2 - 3.0 * a2) / 9.0;

  R = (a1 * (A2 - 4.5 * a2) + 13.5 * a3) / 27.0;

  Q3 = Q * Q * Q;

  R2 = R * R;

  d = Q3 - R2;

  an = a1 / 3.0;

  if (d >= 0.0)
    {
      /* Three real roots. */

      d = R / sqrt (Q3);

      theta = acos (d) / 3.0;

      sQ = -2.0 * sqrt (Q);

      y[0] = sQ * cos (theta) - an;
      y[1] = sQ * cos (theta + TWO_PI_3) - an;
      y[2] = sQ * cos (theta + TWO_PI_43) - an;

      return 3;
    }
  else
    {
      sQ = pow (sqrt (R2 - Q3) + fabs (R), 1.0 / 3.0);

      if (R < 0)
        {
          y[0] = (sQ + Q / sQ) - an;
        }
      else
        {
          y[0] = -(sQ + Q / sQ) - an;
        }

      return 1;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/*!
  \brief Solve a quartic using the method of Francois Vieta (Circa 1735).
*/
int
Polynomial::SolveQuartic (double *results)
{
  double roots[3];
  double c12, z, p, q, q1, q2, r, d1, d2;
  double c0, c1, c2, c3, c4;
  int i;

  // See if the high order term has vanished 
  c0 = coef[4];
  if (fabs (c0) < COEFF_LIMIT)
    {
      return SolveCubic (results);
    }
  // See if the constant term has vanished 
  if (fabs (coef[0]) < COEFF_LIMIT)
    {
      Polynomial y = Polynomial (coef[4], coef[3], coef[2], coef[1]);
      return y.SolveCubic (results);
    }
  // Make sure the quartic has a leading coefficient of 1.0 
  if (c0 != 1.0)
    {
      c1 = coef[3] / c0;
      c2 = coef[2] / c0;
      c3 = coef[1] / c0;
      c4 = coef[0] / c0;
    }
  else
    {
      c1 = coef[3];
      c2 = coef[2];
      c3 = coef[1];
      c4 = coef[0];
    }

  // Compute the cubic resolvant 
  c12 = c1 * c1;
  p = -0.375 * c12 + c2;
  q = 0.125 * c12 * c1 - 0.5 * c1 * c2 + c3;
  r = -0.01171875 * c12 * c12 + 0.0625 * c12 * c2 - 0.25 * c1 * c3 + c4;

  Polynomial cubic;
  cubic[3] = 1.0;
  cubic[2] = -0.5 * p;
  cubic[1] = -r;
  cubic[0] = 0.5 * r * p - 0.125 * q * q;
  cubic.n = 3;

  i = cubic.SolveCubic (roots);
  if (i > 0)
    z = roots[0];
  else
    return 0;

  d1 = 2.0 * z - p;

  if (d1 < 0.0)
    {
      if (d1 > -EPSILON)
        d1 = 0.0;
      else
        return 0;
    }
  if (d1 < EPSILON)
    {
      d2 = z * z - r;
      if (d2 < 0.0)
        return 0;
      d2 = sqrt (d2);
    }
  else
    {
      d1 = sqrt (d1);
      d2 = 0.5 * q / d1;
    }

  // Set up useful values for the quadratic factors 
  q1 = d1 * d1;
  q2 = -0.25 * c1;
  i = 0;

  // Solve the first quadratic
  p = q1 - 4.0 * (z - d2);
  if (p == 0)
    results[i++] = -0.5 * d1 - q2;
  else if (p > 0)
    {
      p = sqrt (p);
      results[i++] = -0.5 * (d1 + p) + q2;
      results[i++] = -0.5 * (d1 - p) + q2;
    }
  // Solve the second quadratic 
  p = q1 - 4.0 * (z + d2);
  if (p == 0)
    results[i++] = 0.5 * d1 - q2;
  else if (p > 0)
    {
      p = sqrt (p);
      results[i++] = 0.5 * (d1 + p) + q2;
      results[i++] = 0.5 * (d1 - p) + q2;
    }
  return i;
}

}