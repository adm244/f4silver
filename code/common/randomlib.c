// Taken from http://paulbourke.net/miscellaneous/random/
// and modified to handle multiple random sequences and
// to be compilable as one unit

/*
  This Random Number Generator is based on the algorithm in a FORTRAN
  version published by George Marsaglia and Arif Zaman, Florida State
  University; ref.: see original comments below.
  At the fhw (Fachhochschule Wiesbaden, W.Germany), Dept. of Computer
  Science, we have written sources in further languages (C, Modula-2
  Turbo-Pascal(3.0, 5.0), Basic and Ada) to get exactly the same test
  results compared with the original FORTRAN version.
  April 1989
  Karl-L. Noell <NOELL@DWIFH1.BITNET>
    and  Helmut  Weber <WEBER@DWIFH1.BITNET>

  This random number generator originally appeared in "Toward a Universal
  Random Number Generator" by George Marsaglia and Arif Zaman.
  Florida State University Report: FSU-SCRI-87-50 (1987)
  It was later modified by F. James and published in "A Review of Pseudo-
  random Number Generators"
  THIS IS THE BEST KNOWN RANDOM NUMBER GENERATOR AVAILABLE.
  (However, a newly discovered technique can yield
  a period of 10^600. But that is still in the development stage.)
  It passes ALL of the tests for random number generators and has a period
  of 2^144, is completely portable (gives bit identical results on all
  machines with at least 24-bit mantissas in the floating point
  representation).
  The algorithm is a combination of a Fibonacci sequence (with lags of 97
  and 33, and operation "subtraction plus one, modulo one") and an
  "arithmetic sequence" (using subtraction).

  Use IJ = 1802 & KL = 9373 to test the random number generator. The
  subroutine RANMAR should be used to generate 20000 random numbers.
  Then display the next six random numbers generated multiplied by 4096*4096
  If the random number generator is working properly, the random numbers
  should be:
          6533892.0  14220222.0  7275067.0
          6172232.0  8354498.0   10633180.0
*/

#ifndef _RANDOM_LIB
#define _RANDOM_LIB

/* Globals */
//internal double u[97],c, cd, cm;
//internal int i97, j97;
//internal int test = FALSE;

struct RandomSequence {
  double u[97];
  double c;
  double cd;
  double cm;
  int i97;
  int j97;
  
  int ij;
  int kl;
  int initialized;
  
  HANDLE mutex;
};

internal RandomSequence DefaultRandomSequence;
internal RandomSequence BatchRandomSequence;
internal RandomSequence TeleportRandomSequence;

/*
  This is the initialization routine for the random number generator.
  NOTE: The seed variables can have values between:    0 <= IJ <= 31328
                                                       0 <= KL <= 30081
  The random number sequences created by these two seeds are of sufficient
  length to complete an entire calculation with. For example, if several
  different groups are working on different parts of the same calculation,
  each group could be assigned its own IJ seed. This would leave each group
  with 30000 choices for the second seed. That is to say, this random
  number generator can create 900 million different subsequences -- with
  each subsequence having a length of approximately 10^30.
*/
internal void RandomInitialize(RandomSequence *seq, int ij, int kl)
{
  double s, t;
  int ii, i, j, k, l, jj, m;
  
  seq->mutex = CreateMutexA(0, FALSE, 0);
  assert(seq->mutex != 0);

  /*
    Handle the seed range errors
    First random number seed must be between 0 and 31328
    Second seed must have a value between 0 and 30081
  */
  if (ij < 0 || ij > 31328 || kl < 0 || kl > 30081) {
    ij = 1802;
    kl = 9373;
  }
  
  DWORD mutexState = WaitForSingleObject(seq->mutex, INFINITE);
  if (mutexState == WAIT_OBJECT_0) {
    seq->ij;
    seq->kl;

    i = (ij / 177) % 177 + 2;
    j = (ij % 177)       + 2;
    k = (kl / 169) % 178 + 1;
    l = (kl % 169);

     for (ii=0; ii<97; ii++) {
        s = 0.0;
        t = 0.5;
      for (jj=0; jj<24; jj++) {
        m = (((i * j) % 179) * k) % 179;
        i = j;
        j = k;
        k = m;
        l = (53 * l + 1) % 169;
        if (((l * m % 64)) >= 32)
          s += t;
        t *= 0.5;
      }
      seq->u[ii] = s;
    }

    seq->c    = 362436.0 / 16777216.0;
    seq->cd   = 7654321.0 / 16777216.0;
    seq->cm   = 16777213.0 / 16777216.0;
    seq->i97  = 97;
    seq->j97  = 33;
    seq->initialized = TRUE;
    
    ReleaseMutex(seq->mutex);
  }
}

internal void RandomInitializeSeed(RandomSequence *seq, uint64 seed)
{
  uint64 ij = seed % 31328;
  uint64 kj = ((seed >> 3) ^ seed) % 30081;
  
  RandomInitialize(seq, ij, kj);
}

/*
  This is the random number generator proposed by George Marsaglia in
  Florida State University Report: FSU-SCRI-87-50
*/
internal double RandomUniform(RandomSequence *seq)
{
  double uni = 0.0;

  //TODO(adm244): rewrite random library, so it's thread-safe
  // and can handle multiple random sequences simultaneously
  DWORD mutexState = WaitForSingleObject(seq->mutex, INFINITE);
  if (mutexState == WAIT_OBJECT_0) {
    /* Make sure the initialisation routine has been called */
    if (!seq->initialized)
      RandomInitialize(seq, 1802, 9373);

    uni = seq->u[seq->i97-1] - seq->u[seq->j97-1];
    if (uni <= 0.0)
      uni++;
    seq->u[seq->i97-1] = uni;
    seq->i97--;
    if (seq->i97 == 0)
      seq->i97 = 97;
    seq->j97--;
    if (seq->j97 == 0)
      seq->j97 = 97;
    seq->c -= seq->cd;
    if (seq->c < 0.0)
      seq->c += seq->cm;
    uni -= seq->c;
    if (uni < 0.0)
      uni++;
  
    ReleaseMutex(seq->mutex);
  }

  return(uni);
}

/*
  ALGORITHM 712, COLLECTED ALGORITHMS FROM ACM.
  THIS WORK PUBLISHED IN TRANSACTIONS ON MATHEMATICAL SOFTWARE,
  VOL. 18, NO. 4, DECEMBER, 1992, PP. 434-435.
  The function returns a normally distributed pseudo-random number
  with a given mean and standard devaiation.  Calls are made to a
  function subprogram which must return independent random
  numbers uniform in the interval (0,1).
  The algorithm uses the ratio of uniforms method of A.J. Kinderman
  and J.F. Monahan augmented with quadratic bounding curves.
*/
internal double RandomGaussian(RandomSequence *seq, double mean, double stddev)
{
  double q, u, v, x, y;

  /*
    Generate P = (u,v) uniform in rect. enclosing acceptance region
    Make sure that any random numbers <= 0 are rejected, since
    gaussian() requires uniforms > 0, but RandomUniform() delivers >= 0.
  */
   do {
    u = RandomUniform(seq);
    v = RandomUniform(seq);
    
    if (u <= 0.0 || v <= 0.0) {
      u = 1.0;
      v = 1.0;
    }
    v = 1.7156 * (v - 0.5);

    /* Evaluate the quadratic form */
    x = u - 0.449871;
    y = fabs(v) + 0.386595;
    q = x * x + y * (0.19600 * y - 0.25472 * x);

    /* Accept P if inside inner ellipse */
    if (q < 0.27597)
    break;

    /* Reject P if outside outer ellipse, or outside acceptance region */
  } while ((q > 0.27846) || (v * v > -4.0 * log(u) * u * u));

  /* Return ratio of P's coordinates as the normal deviate */
  return (mean + stddev * v / u);
}

/*
  Return random integer within a range, lower -> upper INCLUSIVE
*/
internal int RandomInt(RandomSequence *seq, int lower, int upper)
{
  return((int)(RandomUniform(seq) * (upper - lower + 1)) + lower);
}

/*
  Return random integer within a range, min -> (max - 1)
*/
internal int RandomIntExclusive(RandomSequence *seq, int min, int max)
{
  return((int)(RandomUniform(seq) * (max - min)) + min);
}

/*
  Return random float within a range, lower -> upper
*/
internal double RandomDouble(RandomSequence *seq, double lower, double upper)
{
  return((upper - lower) * RandomUniform(seq) + lower);
}

internal float RandomFloat(RandomSequence *seq, float min, float max)
{
  return((max - min) * (float)RandomUniform(seq) + min);
}

internal int RandomIntDefault(int lower, int upper)
{
  return RandomIntExclusive(&DefaultRandomSequence, lower, upper);
}

internal float RandomFloatDefault(float min, float max)
{
  return RandomFloat(&DefaultRandomSequence, min, max);
}

#endif
