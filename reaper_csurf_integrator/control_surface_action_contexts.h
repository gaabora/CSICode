//
//  control_surface_base_actions.h
//  reaper_csurf_integrator
//
//

#ifndef control_surface_action_contexts_h
#define control_surface_action_contexts_h

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class NoAction : public Action
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
public:
    ActionType GetType() const override { return ActionType::NoAction; }
    
    virtual void RequestUpdate(ActionContext *context) override
    {
        context->UpdateColorValue(0.0);
        context->ClearWidget();
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class InvalidAction : public NoAction
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
public:
    ActionType GetType() const override { return ActionType::InvalidAction; }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ReaperAction : public Action
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
public:
    ActionType GetType() const override { return ActionType::ReaperAction; }
   
    virtual void RequestUpdate(ActionContext *context) override
    {
        int state = GetToggleCommandState(context->GetCommandId());
        
        if (state == -1) // this Action does not report state
            state = 0;
        
        if ( ! (context->GetRangeMinimum() == -2.0 || context->GetRangeMaximum() == 2.0)) // used for Increase/Decrease
            context->UpdateWidgetValue(state);
    }
    
    virtual void Do(ActionContext *context, double value) override
    {
        int commandId = context->GetCommandId();
        bool needsReload = context->NeedsReloadAfterRun();
        string commandName = context->GetCommandText();

        // used for Increase/Decrease
        if (value < 0 && context->GetRangeMinimum() < 0)
            DAW::SendCommandMessage(commandId);
        else if (value > 0 && context->GetRangeMinimum() >= 0)
            DAW::SendCommandMessage(commandId);

        if (needsReload) {
            throw ReloadPluginException(commandName);
        }
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FXAction : public Action
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
public:
    ActionType GetType() const override { return ActionType::Abstract; }
    virtual bool IsFxRelated() { return true; }

    virtual bool CheckCurrentContext(ActionContext *context) {
        return context->CheckCurrentFxContext();
    }

    virtual double GetCurrentNormalizedValue(ActionContext* context) override
    {
        if (!CheckCurrentContext(context))
            return 0.0;
        return context->GetTrackFxParamValue();
    }

    virtual void RequestUpdate(ActionContext* context) override
    {
        if (!CheckCurrentContext(context))
            return context->ClearWidget();
        double value = context->GetTrackFxParamValue();
        if (context->IsSameAsLastValue(value))
            return;
        context->UpdateWidgetValue(value);
        context->SetLastValue(value);
    }

    virtual void Do(ActionContext* context, double value) override
    {
        if (context->IsSameAsLastValue(value))
            return;
        if (!CheckCurrentContext(context))
            return;
        context->SetTrackFxParamValue(value);
        if (!context->GetProvideFeedback())
            context->SetLastValue(value);
    }

    virtual void Touch(ActionContext* context, double value) override
    {
        if (value != ActionContext::BUTTON_RELEASE_MESSAGE_VALUE)
            return;
        if (!CheckCurrentContext(context))
            return;
        context->EndTrackFxParamEdit();
    }
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SwitchAction : public Action
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
public:
    ActionType GetType() const override { return ActionType::Abstract; }
    virtual bool IsSwitch() { return true; }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ModifierAction : public Action
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
public:
    ActionType GetType() const override { return ActionType::Abstract; }
    virtual bool IsModifier() { return true; }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class DisplayAction : public Action
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
public:
    ActionType GetType() const override { return ActionType::Abstract; }
    virtual bool IsDisplayRelated() { return true; }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MeterAction : public Action
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
public:
    ActionType GetType() const override { return ActionType::Abstract; }
    virtual bool IsDisplayRelated() { return true; }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class VolumeAction : public Action
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
public:
    ActionType GetType() const override { return ActionType::Abstract; }
    virtual bool IsVolumeRelated() { return true; }
    
    virtual bool CheckCurrentContext(ActionContext *context) {
        return context->CheckCurrentTrackContext();
    }

    virtual double GetCurrentNormalizedValue(ActionContext* context) override 
    {
        if (!CheckCurrentContext(context))
            return 0.0;
        return context->GetTrackVolumeValue();
    }

    virtual void RequestUpdate(ActionContext* context) override
    {
        if (!CheckCurrentContext(context))
            return context->ClearWidget();
        double value = context->GetTrackVolumeValue();
        context->UpdateWidgetValue(value);
        context->SetLastValue(value);
    }

    virtual void Do(ActionContext* context, double value) override
    {
        if (context->IsSameAsLastValue(value))
            return;
        if (!CheckCurrentContext(context))
            return;
        context->SetTrackVolumeValue(value);
        if (!context->GetProvideFeedback())
            context->SetLastValue(value);
    }

    virtual void Touch(ActionContext* context, double value) override
    {
        if (!CheckCurrentContext(context))
            return;
        double currentValue = GetCurrentNormalizedValue(context);
        this->Do(context, currentValue);
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class PanAction : public Action
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
public:
    ActionType GetType() const override { return ActionType::Abstract; }
    virtual bool IsPanRelated() { return true; }
};

#endif /* control_surface_action_contexts_h */
