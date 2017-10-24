close all

clear all

%%

% n = 1000;
% full_rate = 100e3;
% trig_rate = 15e3;
% trigger_window = 18e-6;

% n = 10000000;
n = 100000;
full_rate = 100e3;
trig_rate = 15e3;
% trigger_window = .2e-6;
% trigger_window = 13e-6;
% trigger_window = 17.5e-6;
trigger_window = 35e-6;
BCO = 10e6;

trigger_window_in_BCO = int64(trigger_window * BCO);
event_time_gap = ...
    exprnd(1/full_rate,n,1);

DataLoadBCO = zeros(int64(n/full_rate*BCO*1.1), 1);
TriggerLoadBCO = zeros(int64(n/full_rate*BCO*1.1), 1);

trig_event  = binornd(1,trig_rate./full_rate, n, 1);

%%

recorded_data = 0;
triggered_data = 0;

n_recorded = 0;
n_triggered = 0;
current_trigger_window = -1;
current_time = 0;

for i = 1:n
    
    start_time_bin = int64(current_time*BCO)+1;
    % add data
    DataLoadBCO(start_time_bin:(start_time_bin+trigger_window_in_BCO)) =...
        DataLoadBCO(start_time_bin:(start_time_bin+trigger_window_in_BCO)) + 1;
    
    % trigger
    if (trig_event(i) == 1)
        current_trigger_window = trigger_window;
        
        TriggerLoadBCO(start_time_bin:(start_time_bin+trigger_window_in_BCO)) =...
            TriggerLoadBCO(start_time_bin:(start_time_bin+trigger_window_in_BCO)) + 1;
        
        n_triggered = n_triggered + 1;
        
        % triggered readout
        trigger_search_window = trigger_window;
        event_search = i;
        while (trigger_search_window >=0 && event_search<=n)
            
            if (trigger_search_window>0)
                triggered_data = triggered_data + 1;
            end
            
            trigger_search_window = trigger_search_window - event_time_gap(event_search);
            event_search = event_search + 1;
        end
    end
    
    % throttled readout
    if (current_trigger_window>0)
        n_recorded = n_recorded + 1;
        
        if (current_trigger_window>0)
            
            recorded_data = recorded_data + 1;
            
        end % if (current_trigger_window>=trigger_window)
        
    end % (current_trigger_window>0)
    
    % time elaps
    current_trigger_window = current_trigger_window - event_time_gap(i);
    current_time = current_time + event_time_gap(i);
    
end

%%

% fprintf('Throttled event / total = %.3f; Throttled data / total = %.3f; Triggered event / total = %.3f; Triggered data / total = = %.3f\n',...
%     n_recorded/n, recorded_data/n, n_triggered./n, triggered_data/n);

fprintf('Trigger rate*drift window = %.3f;Full rate*drift window= %.3f;Trigger rate/full rate= %.3f; \n',...
    trig_rate*trigger_window,full_rate*trigger_window,trig_rate/full_rate  );
fprintf('throttled event / total = %.3f; Triggered event / total = = %.3f;  throttled/trigger = %.3f\n',...
    recorded_data/n, triggered_data/n, recorded_data/triggered_data );
fprintf('throttled data / total = %.3f; Triggered data / total = %.3f; throttled/trigger = %.3f \n',...
    sum((TriggerLoadBCO >0) .* DataLoadBCO)/ sum(DataLoadBCO), sum(TriggerLoadBCO .* DataLoadBCO)/ sum(DataLoadBCO), ...
     sum((TriggerLoadBCO >0) .* DataLoadBCO/ sum(TriggerLoadBCO .* DataLoadBCO)));




%%


% recorded_data = 0;
% triggered_data = 0;
%
% n_recorded = 0;
% n_triggered = 0;
% current_trigger_window = -1;
% recorded_window = -1;
%
% for i = 1:n
%
%     % trigger
%     if (trig_event(i) == 1)
%          current_trigger_window = trigger_window;
%     end
%
%     % readout
%     if (current_trigger_window>0)
% %         n_recorded = n_recorded + 1;
% %         n_triggered = n_triggered + 1;
%
%         if (current_trigger_window>=trigger_window)
%
%             if (recorded_window>=trigger_window)
%                 recorded_data = recorded_data +0;
%             elseif (recorded_window>=0)
%                 recorded_data = recorded_data + (trigger_window - recorded_window)/trigger_window;
%                 recorded_window = trigger_window;
%             else
%                 recorded_data = recorded_data + 1;
%                 recorded_window = trigger_window;
%             end
%
%         else % if (current_trigger_window>=trigger_window)
%
%             if (recorded_window>=current_trigger_window)
%                 recorded_data = recorded_data +0;
%             elseif (recorded_window>=0)
%                 recorded_data = recorded_data + (current_trigger_window - recorded_window)/trigger_window;
%                 recorded_window = current_trigger_window;
%             else
%                 recorded_data = recorded_data + (current_trigger_window )/trigger_window;
%                 recorded_window = current_trigger_window;
%             end
%         end % if (current_trigger_window>=trigger_window)
%
%     end % (current_trigger_window>0)
%
%     % time elaps
%     current_trigger_window = current_trigger_window - event_time_gap(i);
%     recorded_window = recorded_window - event_time_gap(i);
%
% end


