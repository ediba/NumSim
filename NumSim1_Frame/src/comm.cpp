#include <mpi.h>
#include "comm.hpp"
#include "grid.hpp"
#include "iterator.hpp"
#include "typedef.hpp"

Communicator::Communicator (int* argc, char*** argv):
_tidx(), _tdim(), _mpi_cart_comm(){
    MPI_Init(argc, argv);
    // Get the rank of the current thread
    MPI_Comm_rank(MPI_COMM_WORLD, &this->_rank);
    // Get total number of threads
    MPI_Comm_size(MPI_COMM_WORLD, &this->_size);
  // compute idx and dim from rank and size
  int dims[DIM], periodic[DIM];
  for(index_t dim = 0; dim < DIM; dim++) {
    dims[dim] = 0;
    periodic[dim] = 0;
  }
  MPI_Dims_create(this->_size, (int)DIM, dims);
  for(index_t dim = 0; dim < DIM; dim++) {
    this->_tdim[dim] = dims[dim];
  }

  MPI_Cart_create(MPI_COMM_WORLD, (int)DIM, dims, periodic, 0, &this->_mpi_cart_comm);
  int coords[DIM];
  MPI_Cart_get(this->_mpi_cart_comm, (int)DIM, dims, periodic, coords);
  this->_evenodd = true;
  for(index_t dim = 0; dim < DIM; dim++) {
    this->_tidx[dim] = coords[dim];
    this->_evenodd ^= ((coords[dim] % 2) == 1);
  }
}
    Communicator::~Communicator (){
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Finalize();
    }

    const multi_index_t& Communicator::ThreadIdx () const{
        return _tidx;
    }
    const multi_index_t& Communicator::ThreadDim () const{
        return _tdim;
    }

    const int&	Communicator::ThreadNum	() const{
        return _rank;
    }
    const int&	Communicator::ThreadCnt	() const{
        return _size;
    }

    const bool&	Communicator::EvenOdd() const{
        return _evenodd;
    }

    // MPI_Gather und MPI_Gatherv sammeln Daten von allen Prozessen ein,
    // ein einzelner Prozeß, der "Besitzer" der Funktion erhält die Daten.
    // MPI_Reduce, MPI_Allreduce und MPI_Scan, wie MPI_Gather(v), nur daß man noch mitteilen muß wie die Daten zu verrechnen sind.

    ///int MPI_Allreduce(void *sendbuf, void *recvbuf, int count, MPI_Datatype type, MPI_Op op, MPI_Comm comm);
    ///Sammelt Daten von allen Prozessen im Kommunikator, verrechnet sie und sendet das Ergebnis an alle Prozesse im Kommunikator.
    real_t Communicator::geatherSum (real_t& val) const{
        real_t sum;
        MPI_Allreduce(&val, &sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        return sum;
    }
    real_t Communicator::geatherMin (real_t& val) const{
        real_t minGather;
        MPI_Allreduce(&val, &minGather, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
        return minGather;
    }
    real_t Communicator::geatherMax (real_t& val) const{
        real_t maxGather;
        MPI_Allreduce(&val, &maxGather, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
        return maxGather;
    }

    void Communicator::copyBoundary (Grid* grid) const{
        //TODO: EvenOdd einbauen, also Kommuikation in 2 Schritten
        if(_size > 1){
            if(!isRight()){
                copyRightBoundary(grid);
            }
            if(!isLeft()){
                copyLeftBoundary(grid);
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
        if (_size > 2){
            if(!isBottom()){
                copyBottomBoundary(grid);
            }
            if(!isTop()){
                copyTopBoundary(grid);
            }

        }
    }

    bool Communicator::copyLeftBoundary(Grid* grid) const{
        index_t size = grid->SizeY()+2;
        real_t buffer[size];
        grid->GetLeftBoundary(buffer);
        int dest,source;
        MPI_Cart_shift(this->_mpi_cart_comm,0,-1,&source,&dest);
        int tag = 0;
        MPI_Status stat;
        MPI_Sendrecv_replace( buffer, size, MPI_DOUBLE, dest, tag, dest, tag, MPI_COMM_WORLD, &stat );
        grid->SetLeftBoundary(buffer);
        return true;

    }
    bool Communicator::copyRightBoundary(Grid* grid) const{
        index_t size = grid->SizeY()+2;
        real_t buffer[size];
        grid->GetRightBoundary(buffer);
        int dest,source;
        MPI_Cart_shift(this->_mpi_cart_comm,0,1,&source,&dest);
        int tag = 0;
        MPI_Status stat;
        MPI_Sendrecv_replace( buffer, size, MPI_DOUBLE, dest, tag, dest, tag, MPI_COMM_WORLD, &stat );
        grid->SetRightBoundary(buffer);
        return true;
    }
    bool Communicator::copyTopBoundary(Grid* grid) const{
        index_t size = grid->SizeX()+2;
        real_t buffer[size];
        grid->GetTopBoundary(buffer);
        int dest,source;
        MPI_Cart_shift(this->_mpi_cart_comm,1,1,&source,&dest);
        int tag = 0;
        MPI_Status stat;
        MPI_Sendrecv_replace( buffer, size, MPI_DOUBLE, dest, tag, dest, tag, MPI_COMM_WORLD, &stat );
        grid->SetTopBoundary(buffer);
        return true;
    }

    bool Communicator::copyBottomBoundary(Grid* grid) const{
        index_t size = grid->SizeX()+2;
        real_t buffer[size];
        grid->GetBotBoundary(buffer);
        int dest,source;
        MPI_Cart_shift(this->_mpi_cart_comm,1,-1,&source,&dest);
        int tag = 0;
        MPI_Status stat;
        MPI_Sendrecv_replace( buffer, size, MPI_DOUBLE, dest, tag, dest, tag, MPI_COMM_WORLD, &stat );
        grid->SetBotBoundary(buffer);
        return true;
    }

    const bool Communicator::isLeft () const{
        return (_tidx[0] == 0);
    }
    const bool Communicator::isRight () const{
        return (_tidx[0] == _tdim[0]-1);
    }
    const bool Communicator::isTop () const{
        return (_tidx[1] == _tdim[1]-1);
    }
    const bool Communicator::isBottom () const{
        return (_tidx[1] == 0);
    }
    //added by the queen

void Communicator::CommunicateStreamLineX(Grid* grid) const{
    if (!isRight()){
        real_t buffer[1];
        grid->BottomRightChange(buffer);
        int dest,source;
        MPI_Cart_shift(this->_mpi_cart_comm,0,-1,&source,&dest);
        int tag = 0;
        MPI_Status stat;
        MPI_Sendrecv_replace( buffer, 1, MPI_DOUBLE, dest, tag, dest, tag, MPI_COMM_WORLD, &stat );
        grid->ReturnBottomLeft(buffer);
    }
}


//added by the queen

   void Communicator::CommunicateStreamLineY(Grid* grid) const{
    if (!isTop()){
        index_t size = grid->SizeX();
        real_t buffer[size];
        grid->GetTopBoundary(buffer);
        int dest,source;
        MPI_Cart_shift(this->_mpi_cart_comm,1,1,&source,&dest);
        int tag = 0;
        MPI_Status stat;
        MPI_Sendrecv_replace( buffer, size, MPI_DOUBLE, dest, tag, dest, tag, MPI_COMM_WORLD, &stat );
        grid->BotBoundaryAdd(buffer);
    }



}
// private:
// 	multi_index_t _tidx;
// 	multi_index_t _tdim;
// 	int _rank;
// 	int _size;
// 	bool _evenodd;
//
//     bool copyLeftBoundary	(Grid* grid) const;
//     bool copyRightBoundary	(Grid* grid) const;
//     bool copyTopBoundary	(Grid* grid) const;
//     bool copyBottomBoundary	(Grid* grid) const;
// };
