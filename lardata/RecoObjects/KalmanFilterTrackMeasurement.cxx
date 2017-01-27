#include "KalmanFilterTrackMeasurement.h"

using namespace trkf;

SVector5     KalmanFilterTrackMeasurement::updatedTrackStatePar() const {
  SVector5 he;
  he.At(comp_) = 1./(meas_->hitMeasErr2()+meas_->trackStateCov().At(comp_,comp_));
  return SVector5(meas_->trackStatePar() + meas_->trackStateCov()*he*(meas_->hitMeas() - meas_->trackStatePar()[comp_]));
}

SMatrixSym55 KalmanFilterTrackMeasurement::updatedTrackStateCov() const {
  SMatrixSym55 tmp;
  tmp.At(comp_,comp_) = 1./(meas_->hitMeasErr2()+meas_->trackStateCov().At(comp_,comp_));
  return (meas_->trackStateCov()-ROOT::Math::Similarity(meas_->trackStateCov(),tmp));
}

recob::TrackState KalmanFilterTrackMeasurement::combinedTrackState(recob::TrackState state1, recob::TrackState state2, bool& success) const {
  const SVector5& par1 = state1.parameters();
  const SVector5& par2 = state2.parameters();
  const SMatrixSym55& cov1 = state1.covariance();
  const SMatrixSym55& cov2 = state2.covariance();
  SMatrixSym55&& cov = cov1 + cov2;
  success = cov.Invert();
  const auto K = cov1 * cov;
  if (!success) return state1;
  SVector5&& par = par1 + K*(par2 - par1);
  cov = SMatrixSym55(SMatrix55(K*cov2).LowerBlock());
  return recob::TrackState(par,cov);
}
