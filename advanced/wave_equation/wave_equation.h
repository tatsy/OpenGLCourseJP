#ifndef _WAVE_EQUATION_H_
#define _WAVE_EQUATION_H_

#include <cstdio>
#include <cstring>

class WaveEquation {
public:
    WaveEquation()
        : xCells_(0)
        , yCells_(0)
        , speed_(0.0)
        , dx_(0.0)
        , dt_(0.0)
        , loss_(0.001)
        , ucurr_(NULL)
        , unext_(NULL)
        , uprev_(NULL) {
    }

    WaveEquation(int xCells, int yCells, double speed,
                 double dx = 0.01, double dt = 0.01)
        : xCells_(xCells)
        , yCells_(yCells)
        , speed_(speed)
        , dx_(dx)
        , dt_(dt)
        , loss_(0.001)
        , ucurr_(NULL)
        , unext_(NULL)
        , uprev_(NULL) {

        allocateMemory();
    }

    WaveEquation(const WaveEquation &weq)
        : xCells_(0)
        , yCells_(0)
        , speed_(0.0)
        , dx_(0.0)
        , dt_(0.0)
        , loss_(0.001)
        , ucurr_(NULL)
        , unext_(NULL)
        , uprev_(NULL) {
        this->operator=(weq);
    }

    virtual ~WaveEquation() {
        delete[] ucurr_;
        delete[] unext_;
        delete[] uprev_;
    }

    WaveEquation & operator=(const WaveEquation &weq) {
        this->xCells_ = weq.xCells_;
        this->yCells_ = weq.yCells_;
        this->speed_ = weq.speed_;
        this->dx_ = weq.dx_;
        this->dt_ = weq.dt_;
        this->loss_ = weq.loss_;

        delete[] ucurr_;
        delete[] uprev_;

        if (weq.ucurr_ != NULL) {
            ucurr_ = new double[xCells_ * yCells_];
            std::memcpy(ucurr_, weq.ucurr_, sizeof(double) * xCells_ * yCells_);
        } else {
            ucurr_ = NULL;
        }

        if (weq.unext_ != NULL) {
            unext_ = new double[xCells_ * yCells_];
            std::memcpy(unext_, weq.unext_, sizeof(double) * xCells_ * yCells_);
        } else {
            unext_ = NULL;
        }

        if (weq.uprev_ != NULL) {
            uprev_ = new double[xCells_ * yCells_];
            std::memcpy(uprev_, weq.uprev_, sizeof(double) * xCells_ * yCells_);
        } else {
            uprev_ = NULL;
        }

        return *this;
    }

    void setParams(int xCells, int yCells, double speed,
                   double dx = 0.01, double dt = 0.01, double loss = 0.001) {
        this->xCells_ = xCells;
        this->yCells_ = yCells;
        this->speed_ = speed;
        this->dx_ = dx;
        this->dt_ = dt;
        this->loss_ = loss;

        allocateMemory();
    }

    void start() {
        std::memcpy(uprev_, ucurr_, sizeof(double) * xCells_ * yCells_);
    }

    void step() {
        static const int NN = 4;
        static const int dx[] = { -1, 1, 0, 0 };
        static const int dy[] = { 0, 0, -1, 1 };

        for (int y = 1; y < yCells_ - 1; y++) {
            for (int x = 1; x < xCells_ - 1; x++) {
                double sum = 0.0;
                for (int i = 0; i < NN; i++) {
                    const int nx = x + dx[i];
                    const int ny = y + dy[i];
                    if (x < 0 || y < 0 || x >= xCells_ || y >= yCells_) {
                        continue;
                    }

                    sum += ucurr_[ny * xCells_ + nx] - ucurr_[y * xCells_ + x];
                }

                unext_[y * xCells_ + x] = ucurr_[y * xCells_ + x]
                                          + (1.0 - loss_) * (ucurr_[y * xCells_ + x] - uprev_[y * xCells_ + x]
                                                             + (speed_ * speed_ * dt_ * dt_ * sum / (dx_ * dx_)));
            }
        }

        // Neumann border condition.
        for (int x = 0; x < xCells_; x++) {
            unext_[0 * xCells_ + x] = -unext_[1 * xCells_ + x];
            unext_[(yCells_ - 1) * xCells_ + x] = -unext_[(yCells_ - 2) * xCells_ + x];
        }

        for (int y = 0; y < yCells_; y++) {
            unext_[y * xCells_ + 0] = -unext_[y * xCells_ + 1];
            unext_[y * xCells_ + (xCells_ - 1)] = -unext_[y * xCells_ + (xCells_ - 2)];
        }

        std::memcpy(uprev_, ucurr_, sizeof(double) * xCells_ * yCells_);
        std::memcpy(ucurr_, unext_, sizeof(double) * xCells_ * yCells_);
    }

    void set(int x, int y, double height) {
        ucurr_[y * xCells_ + x] = height;
    }

    double get(int x, int y) const {
        return ucurr_[y * xCells_ + x];
    }

    double * const heights() const {
        return ucurr_;
    }

private:
    void allocateMemory() {
        delete[] ucurr_;
        delete[] unext_;
        delete[] uprev_;

        ucurr_ = new double[xCells_ * yCells_];
        unext_ = new double[xCells_ * yCells_];
        uprev_ = new double[xCells_ * yCells_];
        std::memset(ucurr_, 0, sizeof(double) * xCells_ * yCells_);
        std::memset(unext_, 0, sizeof(double) * xCells_ * yCells_);
        std::memset(uprev_, 0, sizeof(double) * xCells_ * yCells_);    
    }

    int xCells_, yCells_;
    double speed_, dx_, dt_, loss_;
    double *ucurr_;
    double *unext_;
    double *uprev_;
};

#endif  // _WAVE_EQUATION_H_
