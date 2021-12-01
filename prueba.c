#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

const char* estudiantes = "./instances/YorkMills83.stu";
const char* examenes = "./instances/YorkMills83.exm";

typedef struct {
        char id[20];
        int mis_examenes[60];
        int len_mis_examenes;
} STUDENT;

typedef struct {
    int examen;
    int cantidad_conflictos;
} EXAMEN;

typedef struct {
    int timeslot;
    int examenes_en_timeslot[100];
    int len_examenes_en_timeslot;
} TIMESLOT;

int main(int argc, char ** argv){

    FILE *exm, *stu;
    exm = fopen(examenes, "r");
    stu = fopen(estudiantes, "r");

    if (!exm || !stu) {
        printf("No se han podido abrir los archivos");
        return 1;
    }
    struct stat sb;
    if (stat(examenes, &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }
    char *file_contents = malloc(sb.st_size), *aux = malloc(sb.st_size);

    while (fscanf(exm, "%[^\n ] ", file_contents) != EOF && fscanf(exm, "%[^\n ] ", aux) != EOF) {
        //printf("> %s\n", file_contents);
    }
    int cant_examenes;
    cant_examenes = strtol(file_contents, NULL, 10); //este rescata el ultimo id de examen, que representa la cantidad total de examenes
    
    //-----------------se declara e inicializa la matriz de conflictos--------------------
    int matriz_conflictos[cant_examenes][cant_examenes];
    for(int i=0; i<cant_examenes; i++){
        for(int j=0; j<cant_examenes; j++){
            matriz_conflictos[i][j]=0;
        }
    }
    
    char *id_estudiante = malloc(sb.st_size), *id_examen = malloc(sb.st_size);
    STUDENT s={0};
    STUDENT* students; 
    students = (STUDENT*)malloc(sizeof(STUDENT));
    int len_students=0, len_examenes=0;
    
    //-------------Primera iteracion para construir el arreglo de structs students--------------------
    
    fscanf(stu, "%[^\n ] ", id_estudiante);
    fscanf(stu, "%[^\n ] ", id_examen);
    int examen_id;
    examen_id = strtol(id_examen, NULL, 10);
    strcpy(s.id, id_estudiante);
    s.mis_examenes[len_examenes]=examen_id;
    len_examenes++;
    s.len_mis_examenes = len_examenes;
    students[len_students]=s;
    //----------Construcción arreglo dinamico de struct student------------
    
    while (fscanf(stu, "%[^\n ] ", id_estudiante) != EOF && fscanf(stu, "%[^\n ] ", id_examen) != EOF) {
        examen_id = strtol(id_examen, NULL, 10);
        if (strcmp(id_estudiante, students[len_students].id)==0){
            students[len_students].mis_examenes[len_examenes]=examen_id;
            len_examenes++;
            students[len_students].len_mis_examenes = len_examenes;
        }
        else{
            STUDENT s={0};
            len_students++;
            len_examenes=0;
            students = (STUDENT*)realloc(students, (len_students+1)*sizeof(STUDENT));
            strcpy(s.id, id_estudiante);
            s.mis_examenes[len_examenes]=examen_id;
            len_examenes++;
            s.len_mis_examenes = len_examenes;
            students[len_students]=s;
        }
        
    }
    
    /*printf("len students: %d\n", len_students);

    for(int i=0; i<= len_students; i++){
        printf("id estudiante: %s\n", students[i].id);
        printf("len mis examenes: %d\n", students[i].len_mis_examenes);
        for(int j=0; j<students[i].len_mis_examenes; j++){
            printf("examen a rendir: %d\n", students[i].mis_examenes[j]);
        }
        printf("\n");
    }*/

    //----------------------Armado de matriz de conflictos-----------------------------
    for(int i=0; i<= len_students; i++){
        for(int j=0; j<students[i].len_mis_examenes; j++){
            //printf("examen a rendir: %d\n", students[i].mis_examenes[j]);
            if(students[i].len_mis_examenes>1){
                for(int k=j+1; k<students[i].len_mis_examenes; k++){
                    matriz_conflictos[students[i].mis_examenes[j]-1][students[i].mis_examenes[k]-1] = 1;
                    matriz_conflictos[students[i].mis_examenes[k]-1][students[i].mis_examenes[j]-1] = 1;
                }
            }
            
        }
        //printf("\n");
    }

    /*for(int a=0; a<cant_examenes; a++){
        for(int b=0; b<cant_examenes; b++){
            printf("%d ", matriz_conflictos[a][b]);
        }
        printf("\n");
    }*/
    
    
    //--------------------- Greedy 1 ---------------------
    
    EXAMEN* examenes_array; 
    examenes_array = (EXAMEN*)malloc(cant_examenes*sizeof(EXAMEN)); //arreglo [(id_examnen1,cant_conflictos1), (id_examnen2,cant_conflictos2)]
    int cant_conflictos = 0;
    for(int a=0; a<cant_examenes; a++){
        for(int b=0; b<cant_examenes; b++){
            cant_conflictos += matriz_conflictos[a][b];
        }
        EXAMEN e={0};
        e.examen = a; //aqui esta el cambio de id de examen, el examen 1 pasa a ser 0, el 2 es 1 y asi...
        e.cantidad_conflictos = cant_conflictos;
        examenes_array[a] = e;
        cant_conflictos = 0;
    }

    //ordenar arreglo de examenes y sus conflictos
    EXAMEN* examenes_array_ord; 
    examenes_array_ord = (EXAMEN*)malloc(cant_examenes*sizeof(EXAMEN)); 
    int max_conflictos = -1;
    int posicion_max = -1;

    for(int i=0; i<cant_examenes; i++){
        for(int j=0; j<cant_examenes; j++){
            if (examenes_array[j].cantidad_conflictos > max_conflictos){
                posicion_max = j;
                max_conflictos = examenes_array[j].cantidad_conflictos;
            }
        }
        examenes_array_ord[i] = examenes_array[posicion_max];
        examenes_array[posicion_max].cantidad_conflictos = -10; //con esto, "elimino" el que acabo de agregar
        max_conflictos = -1;
    }
    /*
    printf("-------------Conflictos de examenes ordenados ---------------------\n");
    for (int i=0; i<cant_examenes;i++){
        printf("examen: %d\n", examenes_array_ord[i].examen +1);
        printf("cantidad conflictos: %d\n", examenes_array_ord[i].cantidad_conflictos);
        printf("\n");
    }
    printf("\n");
    */
    //aqui implemento greedy
    TIMESLOT* timeslots_array; //arreglo de TIMESLOTS [{[ex1,ex2],2},{[ex3,ex4,ex5],3}], la posicion es el timeslot
    int cant_timeslots = 1;
    timeslots_array = (TIMESLOT*)malloc(cant_timeslots*sizeof(TIMESLOT));
    TIMESLOT t;
    t.examenes_en_timeslot[0] = examenes_array_ord[0].examen; //primera iteracion para el primer examen, que siempre ira en el TS1
    t.len_examenes_en_timeslot = 1;
    t.timeslot = 0; 
    timeslots_array[0]=t;
    int no_agregar_a_timeslot = 0;
    int no_se_anadio_a_ningun_timeslot = 0;
    int e1, e2, len_examenes_timeslot;
    for(int i=1; i<cant_examenes; i++){ //for para recorrer examenes_array_ord
        e1 = examenes_array_ord[i].examen;
        //printf("e1: %d\n", e1);
        for(int j=0; j<cant_timeslots; j++){ //for para ir recorriendo el timeslots_array
            len_examenes_timeslot = timeslots_array[j].len_examenes_en_timeslot;
            for(int k=0; k<len_examenes_timeslot; k++){
                e2 = timeslots_array[j].examenes_en_timeslot[k];
                //printf("e2: %d\n", e2);
                if(matriz_conflictos[e1][e2]!=0){ //si hay conflicto entre los dos examenes, se activa flag para no agregar el actual examen al timeslot
                    no_agregar_a_timeslot = 1;
                    //printf("examen %d tiene conflicto con examen %d\n", e1,e2);
                }
            }
            if(no_agregar_a_timeslot==0){ //esto quiere decir que si pasa, lo puedo agregar al timeslot actual
                //printf("examen %d no tiene conflicto con timeslot %d\n", e1, j);
                timeslots_array[j].examenes_en_timeslot[len_examenes_timeslot]=e1;
                timeslots_array[j].len_examenes_en_timeslot++;
                no_se_anadio_a_ningun_timeslot = 1;
                no_agregar_a_timeslot = 0;
                break;
            }
            no_agregar_a_timeslot = 0;
        }
        if(no_se_anadio_a_ningun_timeslot==0){ //si no encontro ningun timeslot para asignar, debe crear un nuevo timeslot
            //printf("crear nuevo timeslot para examen %d\n",e1);
            cant_timeslots++;
            timeslots_array = (TIMESLOT*)realloc(timeslots_array, (cant_timeslots)*sizeof(TIMESLOT));
            TIMESLOT t_aux = {0};
            t_aux.timeslot = cant_timeslots-1;
            t_aux.examenes_en_timeslot[0]=e1;
            t_aux.len_examenes_en_timeslot = 1;
            timeslots_array[cant_timeslots-1]=t_aux;
        }
        no_se_anadio_a_ningun_timeslot = 0;
        
    }

    //esto es solo printear la solucion!
    printf("---------------------- SOLUCION CON GREEDY------------------------\n");
    int suma=0, suma_total = 0;
    for(int s=0; s<cant_timeslots; s++){
        printf("timeslot: %d\n", s);
        for(int o=0; o<timeslots_array[s].len_examenes_en_timeslot;o++){
            int examen = timeslots_array[s].examenes_en_timeslot[o];    
            printf("id examen %d\n", timeslots_array[s].examenes_en_timeslot[o]+1);
            for(int g=0; g<timeslots_array[s].len_examenes_en_timeslot;g++){
                int ex = timeslots_array[s].examenes_en_timeslot[g];
                if(g!=o){
                    suma += matriz_conflictos[examen][ex];
                }
            }
            suma_total += suma;
            suma=0;
            
        }
        printf("suma conflictos: %d\n", suma_total);
        printf("\n");
        suma_total=0;
    }
    printf("cantidad de timeslots greedy: %d\n", cant_timeslots);
    printf("\n");
    
    /*
    printf("---------------------- SOLUCION CON GREEDY v2------------------------\n");
    
    //------------------solucion greedy 2-------------------------
    int solucion[cant_examenes];
    int cant_timeslots_actual = 1;
    for(int columna=0; columna<cant_examenes; columna++){
        if(matriz_conflictos[0][columna]==0){
            solucion[columna]=1;
        }
        else{
            solucion[columna]=2;
            cant_timeslots_actual = 2;
        }       
    }

    //----------------Algoritmo greedy para encontrar una solucion inicial-----------------------------
    int flag = 0; //flag que indica si se debe aumentar la cantidad de timeslots actual
    for(int sol=0; sol<cant_examenes; sol++){
        for(int solu=sol+1; solu<cant_examenes; solu++){
            if(solucion[sol]==solucion[solu]){
                if(matriz_conflictos[sol][solu]==1){
                    flag=1;
                    solucion[solu]=cant_timeslots_actual+1;
                }
            }
        }
        if(flag == 1){
            cant_timeslots_actual++;
        }
        flag = 0;
    }
    TIMESLOT* timeslots_array_gredy2;
    timeslots_array_gredy2 = (TIMESLOT*)malloc(cant_timeslots_actual*sizeof(TIMESLOT));
    int largo_cant_examenes_timeslot = 0;
    for(int j=1; j<=cant_timeslots_actual; j++){
        TIMESLOT tt_aux = {0};
        tt_aux.timeslot = j-1;
        printf("timeslot %d :\n", j);
        for(int i=0; i<cant_examenes; i++){
            if(solucion[i]==j){
                printf("prueba %d\n", i+1);
                tt_aux.examenes_en_timeslot[largo_cant_examenes_timeslot]=i;
                tt_aux.len_examenes_en_timeslot ++;
            }
            
        }
        timeslots_array_gredy2[j]=tt_aux;
        printf("\n");
    } 
    
    //printf("cantidad examenes: %d\n", cant_examenes);
    //printf("cantidad de timeslots con greedy 2: %d\n", cant_timeslots_actual);
    //printf("len students: %d\n", len_students);
    //printf("len examenes estudiante %s : %d\n", students[175].id, students[175].len_mis_examenes);
    //printf("examen %d\n", students[175].mis_examenes[2]);
    */
    //----------------------- Hill Climbing----------------------------------------

    /*tomamos el timeslot con mas examenes agendados
    tomamos el examen mas conflictivo de ese timeslot y lo asigno en el primer timeslot que no tenga conflito

    el orden siempre es de mas a menos examenes agendados 
    tomo solo 1 examen por timeslot por pasada, hasta que la "matriz solucion" este vacia
    cambiar el examen mas conflictivo -> movimiento
    */

    //ordena la solucion actual por timeslots, desde el timeslot con mas examenes al que menos tiene
    TIMESLOT* timeslots_ord; //arreglo descendente por cantidad de examenes en timeslot
    int orden_timeslots[cant_timeslots]; //guarda el timeslot correspondiente al orden, [2,3,1] quiere decir ts2>ts3>ts1
    timeslots_ord = (TIMESLOT*)malloc(cant_timeslots*sizeof(TIMESLOT));
    int max_cant_examenes_por_timeslot = -1;
    int timeslot_con_mas_examenes = -1;
    for(int i=0; i<cant_timeslots; i++){//for para recorrer timeslots_ord
        for(int j=0; j<cant_timeslots; j++){//for para recorrer timeslots_array
            if(timeslots_array[j].len_examenes_en_timeslot > max_cant_examenes_por_timeslot){
                timeslot_con_mas_examenes = j;
                max_cant_examenes_por_timeslot = timeslots_array[j].len_examenes_en_timeslot;
            }
        }
        TIMESLOT T = {0};
        for(int k=0; k<timeslots_array[timeslot_con_mas_examenes].len_examenes_en_timeslot; k++){ //for para copiar el arreglo de examenes en el timeslot
            T.examenes_en_timeslot[k]=timeslots_array[timeslot_con_mas_examenes].examenes_en_timeslot[k];
        }
        T.len_examenes_en_timeslot=timeslots_array[timeslot_con_mas_examenes].len_examenes_en_timeslot;
        T.timeslot = timeslot_con_mas_examenes;
        orden_timeslots[i]=timeslot_con_mas_examenes;
        timeslots_ord[i]=T;
        timeslots_array[timeslot_con_mas_examenes].len_examenes_en_timeslot = -10; //con esto lo "elimino". en esta linea dejo inutilizable timeslot_array
        timeslot_con_mas_examenes = -1;
        max_cant_examenes_por_timeslot = -1;
    }
    //esto es para printear timeslots ordenados
    /*printf("\n");
    printf("------------- timeslots ordenados ------------------------\n");
    for(int i=0; i<cant_timeslots; i++){
        printf("timeslot: %d\n", timeslots_ord[i].timeslot);
        printf("cantidad examenes: %d\n", timeslots_ord[i].len_examenes_en_timeslot);
        printf("\n");
    }*/

    //aqui empieza el algoritmo HC
    TIMESLOT* sol;
    sol = (TIMESLOT*)malloc(sizeof(TIMESLOT));
    int cant_timeslots_HC = 0;
    int ee1, exa1;
    int max_cant_conflictos = -1;
    int conflictos_ee1;
    int ex_con_mas_conflictos;
    int index_e1_conflictos; //index para ir "eliminando" los examenes con mas conflictos en examenes_array_ord
    int index_ex_con_mas_conflictos;
    int cant_examenes_reubicados=0;
    no_agregar_a_timeslot = 0;
    no_se_anadio_a_ningun_timeslot = 0;
    while(cant_examenes_reubicados<cant_examenes){
        for(int i=0; i<cant_timeslots; i++){//for para recorrer timeslots_ord
            if(timeslots_ord[i].len_examenes_en_timeslot>0){
                for(int j=0; j<timeslots_ord[i].len_examenes_en_timeslot; j++){//for para ir recorriendo los examenes_en_timeslot
                    ee1 = timeslots_ord[i].examenes_en_timeslot[j];
                    for (int k=0; k<cant_examenes; k++){//for para recorrer examenes_array_ord para buscar la cantidad de conflictos del examen
                        if(examenes_array_ord[k].examen == ee1){
                            conflictos_ee1 = examenes_array_ord[k].cantidad_conflictos;
                            break;
                        }
                    }
                    if(conflictos_ee1 > max_cant_conflictos){ //guardo el examen del timeslot con mas conflictos
                        max_cant_conflictos = conflictos_ee1;
                        ex_con_mas_conflictos = ee1;
                        index_ex_con_mas_conflictos = j;
                    }             
                }
                max_cant_conflictos = -1 ; //tengo dudas
                //aqui tengo que eliminar e1  de examenes_en_timeslot
                for(int k=index_ex_con_mas_conflictos; k<timeslots_ord[i].len_examenes_en_timeslot; k++){
                    timeslots_ord[i].examenes_en_timeslot[k]=timeslots_ord[i].examenes_en_timeslot[k+1];
                }
                timeslots_ord[i].len_examenes_en_timeslot-=1;
                //aqui ingreso a la solucion a ex_con_mas_conflictos
                if (cant_timeslots_HC == 0){ //solucion inicial, creo el primer timeslot
                    TIMESLOT tt = {0};
                    tt.timeslot = 0;
                    tt.examenes_en_timeslot[0]=ex_con_mas_conflictos;
                    tt.len_examenes_en_timeslot = 1;
                    sol[cant_timeslots_HC]=tt;
                    cant_timeslots_HC ++;
                }
                else{
                    for(int q=0; q<cant_timeslots_HC; q++){//for para recorrer los timeslots de la solucion actual
                        len_examenes_timeslot = sol[q].len_examenes_en_timeslot;
                        for(int h=0; h<sol[q].len_examenes_en_timeslot; h++){//recorro los examenes actuales
                            exa1 = sol[q].examenes_en_timeslot[h];
                            if(matriz_conflictos[ex_con_mas_conflictos][exa1] != 0){
                                no_agregar_a_timeslot = 1;
                                //printf("examen %d tiene conflicto con examen %d\n", ex_con_mas_conflictos,exa1);
                            }
                        }
                        if(no_agregar_a_timeslot==0){//esto quiere decir que si pasa, lo puedo agregar al timeslot actual
                            //printf("examen %d no tiene conflicto con timeslot %d\n", ex_con_mas_conflictos, q);
                            sol[q].examenes_en_timeslot[len_examenes_timeslot]=ex_con_mas_conflictos;
                            sol[q].len_examenes_en_timeslot++;
                            no_se_anadio_a_ningun_timeslot = 1;
                            no_agregar_a_timeslot=0;
                            break;
                        }
                        no_agregar_a_timeslot=0;
                    }
                    if(no_se_anadio_a_ningun_timeslot==0){//si no encontro ningun timeslot para asignar, debe crear un nuevo timeslot
                        //printf("crear nuevo timeslot para examen %d\n",ex_con_mas_conflictos);
                        cant_timeslots_HC++;
                        sol = (TIMESLOT*)realloc(sol, (cant_timeslots_HC)*sizeof(TIMESLOT));
                        TIMESLOT ts_aux = {0};
                        ts_aux.timeslot = cant_timeslots_HC-1;
                        ts_aux.examenes_en_timeslot[0]=ex_con_mas_conflictos;
                        ts_aux.len_examenes_en_timeslot = 1;
                        sol[cant_timeslots_HC-1]=ts_aux;
                    }
                    no_se_anadio_a_ningun_timeslot = 0;
                }
                cant_examenes_reubicados++;
            }
        }
    }

    printf("\n");
    printf("---------------------- SOLUCION CON HC------------------------\n");

    suma=0; 
    suma_total = 0;
    for(int s=0; s<cant_timeslots_HC; s++){
        printf("timeslot: %d\n", s);
        for(int o=0; o<sol[s].len_examenes_en_timeslot;o++){
            int examen = sol[s].examenes_en_timeslot[o];    
            printf("id examen %d\n", sol[s].examenes_en_timeslot[o]+1);
            for(int g=0; g<sol[s].len_examenes_en_timeslot;g++){
                int ex = sol[s].examenes_en_timeslot[g];
                if(g!=o){
                    suma += matriz_conflictos[examen][ex];
                }
            }
            suma_total += suma;
            suma=0;
            
        }
        printf("suma conflictos: %d\n", suma_total);
        printf("\n");
        suma_total=0;
    }
    printf("cantidad de timeslots HC: %d\n", cant_timeslots_HC);
    printf("cantidad de timeslots greedy: %d\n", cant_timeslots);



    fclose(exm);
    fclose(stu);
    return 0;
}