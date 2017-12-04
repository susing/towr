/**
 @file    contact_schedule.h
 @author  Alexander W. Winkler (winklera@ethz.ch)
 @date    Apr 5, 2017
 @brief   Brief description
 */

#ifndef XPP_OPT_INCLUDE_XPP_OPT_CONTACT_SCHEDULE_H_
#define XPP_OPT_INCLUDE_XPP_OPT_CONTACT_SCHEDULE_H_

#include <memory>
#include <string>
#include <vector>

#include <xpp_states/endeffectors.h>

#include <opt_solve/composite.h>
#include <xpp_opt/variables/phase_nodes.h>

namespace xpp {

class ContactSchedule : public opt::Variable {
public:
  using Ptr           = std::shared_ptr<ContactSchedule>;
  using VecDurations  = std::vector<double>;
  using PhaseNodesPtr = std::shared_ptr<PhaseNodes>;

  ContactSchedule (EndeffectorID ee,
                   double t_total,
                   const VecDurations& timings,
                   bool is_in_contact_at_start,
                   double min_phase_duration,
                   double max_phase_duration);
  virtual ~ContactSchedule () = default;

  bool IsInContact(double t_global) const;

  std::vector<double> GetTimePerPhase() const;
  std::vector<bool> GetContactSequence() const;

  void AddObserver(const PhaseNodesPtr& o);
  void UpdateObservers() const;

  virtual VectorXd GetValues() const override;
  virtual void SetVariables(const VectorXd&) override;
  VecBound GetBounds () const override;

  Jacobian GetJacobianOfPos(double t_global, const std::string& observer_name) const;

  int GetPhaseCount() const { return GetTimePerPhase().size(); };
  bool GetContact(int phase) const;

  double GetTotalTime() const;

private:
  bool first_phase_in_contact_;
  double t_total_;
  opt::Bounds phase_duration_bounds_;

  std::vector<PhaseNodesPtr> observers_;
  PhaseNodesPtr GetObserver(const std::string& id) const;

  VecDurations durations_;




};


// spring_clean_ use own file for this
/** Makes sure all phase durations sum up to final specified motion duration.
 */
class DurationConstraint : public opt::Constraint {
public:
  using SchedulePtr = std::shared_ptr<ContactSchedule>;

  DurationConstraint(const VariablesPtr& opt_vars, double T_total, int ee);
  ~DurationConstraint() = default;

  VectorXd GetValues() const override;
  VecBound GetBounds() const override;
  void FillJacobianBlock (std::string var_set, Jacobian&) const override;

private:
  SchedulePtr schedule_;
  double T_total_;
};


} /* namespace xpp */

#endif /* XPP_OPT_INCLUDE_XPP_OPT_CONTACT_SCHEDULE_H_ */
