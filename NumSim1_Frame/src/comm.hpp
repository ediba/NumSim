#include "grid.hpp"
#include "typedef.hpp"
#include <mpi.h>
//------------------------------------------------------------------------------
#ifndef __COMM_HPP
#define __COMM_HPP
//------------------------------------------------------------------------------
class Communicator {
public:
    ///Constructor
    Communicator (int* argc, char*** argv);
    
    ///Destructor 
    ~Communicator ();
    
    ///cartesian coordinates of thread
    const multi_index_t& ThreadIdx () const;
    
    ///cartesian coordinates total
    const multi_index_t& ThreadDim () const;
    
    ///rank  of the curent threat
    const int&	ThreadNum() const;
    
    ///Number of threads 
    const int&	ThreadCnt() const;
    
    //Checks if the Thread is even or odd
    const bool&	EvenOdd() const;
    
    /// returns the Sum of a value from all Thread 
    real_t geatherSum (real_t& val) const;
    
    /// returns the minimal value from all threads
    real_t geatherMin (real_t& val) const;
    
    /// returns the maximal value of all threads
    real_t geatherMax (real_t& val) const;
    
    /// exchanges the boundaries where it is needed (not at the outer boundaries)
    void copyBoundary(Grid* grid) const;
    
    /// Checks if the current thread is on the left boundary
    const bool isLeft () const;
    
    /// Checks if the current thread is on the right boundary
    const bool isRight () const;
    
    /// Checks if the current thread is on the top boundary
    const bool isTop () const;
    
    /// Checks if the current thread is on the bottom boundary
    const bool isBottom () const;
    
    ///Sums up the values of the stream line in x-direction
    void CommunicateStreamLineX(Grid* grid) const;
    
    ///Sums up the values of the stream line in y-direction
    void CommunicateStreamLineY(Grid* grid) const;
private:
    MPI_Comm _mpi_cart_comm;
    multi_index_t _tidx;
    multi_index_t _tdim;
    int _rank;
    int _size;
    bool _evenodd;
    /// sends and receives the left boundary
    bool copyLeftBoundary (Grid* grid) const;
    /// sends and receives the right boundary
    bool copyRightBoundary (Grid* grid) const;
    /// sends and receives the top boundary
    bool copyTopBoundary (Grid* grid) const;
    /// sends and receives the bottom boundary
    bool copyBottomBoundary (Grid* grid) const;
};
//------------------------------------------------------------------------------
#endif // __COMM_HPP
//------------------------------------------------------------------------------
