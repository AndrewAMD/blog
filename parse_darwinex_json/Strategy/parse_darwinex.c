#include <zjsmn.c>

bool key_matches(const char* content, jsmntok_t* pToken, const char* key, float* valueOutput){
	char this_key[32], this_value[32];
	memcpy(this_key, content + pToken[0].start, pToken[0].end - pToken[0].start);
	if(!strcmp(this_key,key)){  // we have a match
		// check next token for value, must be primitive
		if(pToken[1].type != JSMN_PRIMITIVE){
			return false;
		}
		memcpy(
			this_value, 
			content + pToken[1].start,
			pToken[1].end - pToken[1].start
		);
		*valueOutput = (float)atof(this_value);
		return true;
	}
	else{ //mismatch
		return false;
	}
}

DATE convertTime(float t_unix)
{
  return (double)t_unix/(24.*60.*60.) + 25569.; // 25569. = DATE(1.1.1970 00:00)}
}

int main(){
	const char* filename = ".\\History\\scs_1d.json";
	
	int len = file_length(filename);
	if(!len){
		printf("\nfile: %s not found",filename);
		return 0;
	}
	char* content = malloc(len + 1);
	memset(content,0,len+1);
	if(!file_read(filename,content,len)){
		printf("\n file: %s read error",filename);
		free(content);
		return 0;
	}
	
	// make a new data set handle
	int h = 1;
	
	// set up token array, must be large enough
	#define NUM_TOKENS 100000
	jsmn_parser parser;
	jsmntok_t tokens[NUM_TOKENS];
	jsmn_init(&parser);
	
	// time to tokenize...
	int num_tokens = jsmn_parse(&parser,content,strlen(content),tokens,NUM_TOKENS);
	
	// data entry buffer
	T6 t6;
	memset(&t6,0,sizeof(T6));
	
	// read loop
	int i = 0; // token index
	int field_index = 0; // 0=close, 1=max, 2=min, 3=open
	int num_entries = 0;
	for( 
	;
	i < (num_tokens-1);  // leave room to check next token (value for a key)
	i++
	){
		// if not a key, skip. Keys are strings.
		if(tokens[i].type != JSMN_STRING)
			continue;
		
		
		switch(field_index){
		case 0: // close
			if(key_matches(content, &tokens[i], "close", &t6.fClose)){
				field_index = 1;
			}
			break;
		case 1: // max
			if(key_matches(content, &tokens[i], "max", &t6.fHigh)){
				field_index = 2;
			}
			break;
		case 2: // min
			if(key_matches(content, &tokens[i], "min", &t6.fLow)){
				field_index = 3;
			}
			break;
		case 3: // open
			if(key_matches(content, &tokens[i], "open", &t6.fOpen)){
				field_index = 4;
			}
			break;
		case 4: // timestamp, need to add entry
			float t_unix = 0;
			if(key_matches(content, &tokens[i], "timestamp", &t_unix)){
				t6.time = round(convertTime(t_unix)); // daily bars: must be rounded to nearest int.
				T6* new_row = dataAppendRow(h,7);
				if(!new_row){
					printf("\ndataAppendRow failed!");
					free(content);
					return 0;
				}
				memcpy(new_row, &t6, sizeof(T6));
				num_entries++;
				
				//cleanup
				memset(&t6,0,sizeof(T6));
				field_index = 0;
			}
			break;
		default:
			
		}
	}
	
	printf("\n# entries parsed: %d", num_entries);
	
	printf("\nsorting data from newest to oldest...");
	dataSort(h);	
	printf("\nsaving file: .\\History\\scs_1d.t6");
	dataSave(h,".\\History\\scs_1d.t6");

	free(content);
	printf("\nall done!");
	return 0;
	
}







