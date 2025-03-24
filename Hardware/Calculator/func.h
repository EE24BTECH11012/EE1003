/* func.c */
double rk4_1(double x, double y, double x0, double (*function)(double, double));
double rk4_2(double x, double y1, double y2, double x0, double (*f1)(double, double, double), double (*f2)(double, double, double));
double compute_scientific_functions(double value, int mode);
void handle_shift_mode(char *key, char *expression, int *expr_index);
void handle_alpha_mode(char *key, char *expression, int *expr_index);
void handle_special_functions(char *key, char *expression, int *expr_index);
double factorial(int n);
double power(float base, int exp);
void evaluate_expression(char *expression);
