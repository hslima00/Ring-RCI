bool verify_ip(char* ip_string){ 

	char delim[] = ".";
    char *string_to_free,*ip_string_copy;
    //char string_to_free_="\0";
    
    string_to_free=ip_string_copy=strdup(ip_string);
    char *ptr = strtok(ip_string_copy, delim);
    int point_counter=0;
    
    while(ptr != NULL)
	{
        if(atoi(ptr)<0 || atoi(ptr)>255 || point_counter > 4){
            printf("IP not valid\n");
            return 1;
        }
        point_counter++;
		ptr = strtok(NULL, delim);
	}

    //free(string_to_free);
    bzero(&string_to_free, sizeof(string_to_free));
    return 0;
    
}