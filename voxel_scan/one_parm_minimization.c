
#include  <def_mni.h>

public  void  one_parameter_minimization( parameters, which_parameter,
                                          u_min, u_max, v_min, v_max,
                                          max_delta_parameter,
                                          delta_parameter,
                                          distances_without_this_parameter,
                                          evaluate_fit_function,
                                          evaluation_ptr, fitting_gain )
    double          parameters[];
    int             which_parameter;
    double          u_min;
    double          u_max;
    double          v_min;
    double          v_max;
    double          max_delta_parameter;
    double          *delta_parameter;
    Real            distances_without_this_parameter[];
    double          (*evaluate_fit_function)( void *, double [],
                                  double, double, double, double, Real [] );
    void            *evaluation_ptr;
    double          *fitting_gain;
{
    const    double  TOLERANCE   = 1.0e-4;
    Boolean  done, halved;
    double   parameter_value, fit_here, fit_before, fit_after;
    double   next_fit_before, next_fit_after;

    parameter_value = parameters[which_parameter];

    fit_here = (*evaluate_fit_function) ( evaluation_ptr, parameters,
                                u_min, u_max, v_min, v_max,
                                distances_without_this_parameter );

    done = FALSE;
    halved = FALSE;

    while( !done )
    {
        parameters[which_parameter] = parameter_value - *delta_parameter;
        fit_before = (*evaluate_fit_function) ( evaluation_ptr, parameters,
                                u_min, u_max, v_min, v_max,
                                distances_without_this_parameter );

        parameters[which_parameter] = parameter_value + *delta_parameter;
        fit_after = (*evaluate_fit_function) ( evaluation_ptr, parameters,
                                u_min, u_max, v_min, v_max,
                                distances_without_this_parameter );

        if( fit_after < fit_here || fit_before < fit_here )
        {
            done = TRUE;
        }
        else if( numerically_close( parameter_value,
                                    parameter_value + *delta_parameter,
                                    TOLERANCE ) )
        {
            done = TRUE;
        }
        else
        {
            *delta_parameter /= 2.0;
            halved = TRUE;
        }
    }

    if( !halved && (fit_after < fit_here || fit_before < fit_here) )
    {
        done = FALSE;

        while( !done &&
               (2.0 * *delta_parameter) <= max_delta_parameter )
        {
            parameters[which_parameter] = parameter_value -
                                          *delta_parameter * 2.0;
            next_fit_before = (*evaluate_fit_function) 
                              ( evaluation_ptr, parameters,
                                u_min, u_max, v_min, v_max,
                                distances_without_this_parameter );

            parameters[which_parameter] = parameter_value +
                                          *delta_parameter * 2.0;
            next_fit_after = (*evaluate_fit_function) 
                              ( evaluation_ptr, parameters,
                                u_min, u_max, v_min, v_max,
                                distances_without_this_parameter );

            if( next_fit_after >= fit_here && next_fit_before >= fit_here )
                done = TRUE;
            else
            {
                *delta_parameter *= 2.0;
                fit_after = next_fit_after;
                fit_before = next_fit_before;

                if( *delta_parameter >= max_delta_parameter )
                    done = TRUE;
            }
        }
    }

    if( fit_after < fit_before && fit_after < fit_here )
    {
        parameters[which_parameter] = parameter_value + *delta_parameter;
        *fitting_gain = fit_here - fit_after;
    }
    else if( fit_before < fit_after && fit_before < fit_here )
    {
        parameters[which_parameter] = parameter_value - *delta_parameter;
        *fitting_gain = fit_here - fit_before;
    }
    else
    {
        parameters[which_parameter] = parameter_value;
        *fitting_gain = 0.0;
    }
}