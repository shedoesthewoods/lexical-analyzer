#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int karsilastir(char *array1, char *array2){
    char *p1 = array1;
    char *p2 = array2;

    while(*p1 != '\0'){
        if((*p2 == '\0') || (*p2 > *p1) || (*p1 > *p2))
            return 1;
        p1++;
        p2++;
    }
    if (*p2 != '\0') return 1;

    return 0;
}

int main(){
    FILE *file, *fileLex;
    file = fopen("code.psi", "r");
    fileLex = fopen("code.lex", "w");

    char keywords[18][30];
    strcpy(keywords[0], "break");
    strcpy(keywords[1], "goto");
    strcpy(keywords[2], "case");
    strcpy(keywords[3], "char");
    strcpy(keywords[4], "const");
    strcpy(keywords[5], "continue");
    strcpy(keywords[6], "do");
    strcpy(keywords[7], "else");
    strcpy(keywords[8], "enum");
    strcpy(keywords[9], "float");
    strcpy(keywords[10], "for");
    strcpy(keywords[11], "if");
    strcpy(keywords[12], "int");
    strcpy(keywords[13], "long");
    strcpy(keywords[14], "record");
    strcpy(keywords[15], "return");
    strcpy(keywords[16], "static");
    strcpy(keywords[17], "while");

    char kelime[30];
    char strings[100];

    int i = 0, j = 0, k = 0, m = 0;

    int kontrol = 1;
    int id_len = 0, int_len = 0;


    char sth = getc(file);
    if(sth == -1){
        printf("File not found!");
        return 1;
    }
    while(!feof(file)){
        if(sth == 32 || (8 < sth && sth < 14)){ //Bosluk karakterleri dikkate alinmamalidir
            sth = getc(file);
        }
        //identifier ve keyword blogu
        if((96 < sth && sth <123) || (64 < sth && sth < 91)){ //Alinan karakter buyuk ya da kucuk harf
            i = 0;
            j = 0;
            id_len = 0;
            kontrol = 1;
            while((96 < sth && sth <123) || (64 < sth && sth < 91) || (47 < sth && sth < 58)|| sth == 95){ //Buyuk kucuk harfler, alt çizgi ve rakamlar
                if(i > 30){
                    printf("Identifier names can't be longer than 30 characters!");
                    return 1;
                }
                if((47 < sth && sth < 58)|| sth == 95){ //Eger alınan karakter sayi ya da _ ise identifier
                    kontrol = 0;
                }
                if(64 < sth && sth < 91){ //Alinan karakter büyük harfse küçük harfe çevrilir
                    sth += 32;
                }
                kelime[i] = sth;
                sth = getc(file);
                i++;
            }
            kelime[i] = '\0'; //Kelimenin sonuna null karakter eklendi
            id_len = i;
            if(kontrol == 1){ //Karakterlerin tamamı harf ise keyword mu degil mi kontrolu yapılır
                while(j < 18){
                    if(karsilastir(kelime, keywords[j]) == 0){ //Stringler aynı ise
                        fputs("Keyword(", fileLex);
                        kontrol = 1;
                        break;
                    }
                    else{
                        kontrol = 0;
                    }
                    j++;
                }
            }
            if(kontrol == 0){ //Karakterlerin içinde sayi ya da _ varsa ya da keywordle eşleşmediyse
                fputs("Identifier(", fileLex);
            }
            j = 0;
            while(j < id_len){
                fputc(kelime[j], fileLex);
                j++;
            }
            fputs(")\n", fileLex);
        }
        //operator blokları
        if(sth == 43){ // + operatörü için + ya da ++ olma durumu kontrolü
            sth = getc(file);
            if(sth == 43){
                fputs("Operator(++)\n", fileLex);
            }
            else{
                fputs("Operator(+)\n", fileLex);
                continue;
            }
        }
        if(sth == 45){ // - operatörü için - ya da -- olma durumu kontrolü
            sth = getc(file);
            if(sth == 45){
                fputs("Operator(--)\n", fileLex);
            }
            else{
                fputs("Operator(-)\n", fileLex);
                continue;
            }
        }
        if(sth == 42){
          fputs("Operator(*)\n", fileLex);
        }
        if(sth == 58){
            sth = getc(file);
            if(sth == 61){
                fputs("Operator(:=)\n",fileLex);
            }else{
                printf("Undefined input : character!");
                return 1;
            }
        }
        if(sth == 47){ //alinan karakter bölüm isareti
            sth = getc(file);
            if(sth == 42){ //bölümden sonra yildiz geldi mi
                while(!feof(file)){
                    sth = getc(file);
                    if(sth == 42){ //yildiz geldiyse sonraki karakter bölüm ise yorum sonu demektir
                        sth = getc(file);
                        if(sth == 47) //İkinci yildizdan sonra bolum geldi mi
                            break;
                    }
                    if(sth == EOF){
                        printf("Unterminated comment!");
                        return 1;
                    }
                }
            }
            else{
                fputs("Operator(/)\n", fileLex);
                continue;
            }
        }
        //string blogu
        if(sth == 34){ //Alınan karakter çift tırnak
            sth = getc(file);
            strings[k] = sth;
            k++;
            while(sth != 34 && sth != EOF){
                sth = getc(file);
                strings[k] = sth;
                k++;
            }
            strings[k] = '\0'; //String sonuna null karakteri eklendi
            if(sth == 34){ //Tırnak kapanırsa
                fputs("StrConst(", fileLex);
                while(m < (strlen(strings)-1)){
                    fputc(strings[m], fileLex);
                    m++;
                }
                fputs(")\n", fileLex);
            }
            else if(sth == EOF){ //Tırnak kapanmadan dosya biterse
                printf("Missing terminating \" character!");
                return 1;
            }
        }
        //Integer blogu
        if(47 < sth && sth < 58){ //kontrol edilen, rakamlarin ascii kodlari araligi
            fputs("IntConst(", fileLex);
            while (47 < sth && sth < 58){
                if(int_len > 10){
                    printf("Integers can't be longer than 10 characters!");
                    return 1;
                }
                fputc(sth, fileLex);
                sth = getc(file);
                int_len++;
            }
            fputs(")\n", fileLex);
            int_len = 0;
            continue;
        }
        if(sth == 59){ // ; karakteri kontrolü
            fputs("EndOfLine\n",fileLex);
        }
        //Parantez karakterleri blokları
        if(sth == 40){
            fputs("LeftParantheses\n", fileLex);
        }
        if(sth == 41){
           fputs("RightParantheses\n", fileLex);
        }
        if(sth == 91){
           fputs("LeftSquareBracket\n", fileLex);
        }
        if(sth == 93){
           fputs("RightSquareBracket\n", fileLex);
        }
        if(sth == 123){
           fputs("LeftCurlyBracket\n", fileLex);
        }
        if(sth == 125){
           fputs("RightCurlyBracket\n", fileLex);
        }
        sth = getc(file);
    }
    fclose(file);
    fclose(fileLex);
    fflush(stdin);
    return 0;
}
