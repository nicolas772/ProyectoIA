#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

const char* estudiantes = "./instances/TorontoE92.stu";
const char* examenes = "./instances/TorontoE92.exm";

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

TIMESLOT* ordenar_timeslots(int cant_timeslots, TIMESLOT* ts_desordenado){
    //ordena la solucion actual por timeslots, desde el timeslot con mas examenes al que menos tiene
    TIMESLOT* timeslots_ord; //arreglo descendente por cantidad de examenes en timeslot
    int orden_timeslots[cant_timeslots]; //guarda el timeslot correspondiente al orden, [2,3,1] quiere decir ts2>ts3>ts1
    timeslots_ord = (TIMESLOT*)malloc(cant_timeslots*sizeof(TIMESLOT));
    int max_cant_examenes_por_timeslot = -1;
    int timeslot_con_mas_examenes = -1;
    for(int i=0; i<cant_timeslots; i++){//for para recorrer timeslots_ord
        for(int j=0; j<cant_timeslots; j++){//for para recorrer timeslots_array
            if(ts_desordenado[j].len_examenes_en_timeslot > max_cant_examenes_por_timeslot){
                timeslot_con_mas_examenes = j;
                max_cant_examenes_por_timeslot = ts_desordenado[j].len_examenes_en_timeslot;
            }
        }
        TIMESLOT T = {0};
        for(int k=0; k<ts_desordenado[timeslot_con_mas_examenes].len_examenes_en_timeslot; k++){ //for para copiar el arreglo de examenes en el timeslot
            T.examenes_en_timeslot[k]=ts_desordenado[timeslot_con_mas_examenes].examenes_en_timeslot[k];
        }
        T.len_examenes_en_timeslot=ts_desordenado[timeslot_con_mas_examenes].len_examenes_en_timeslot;
        T.timeslot = timeslot_con_mas_examenes;
        orden_timeslots[i]=timeslot_con_mas_examenes;
        timeslots_ord[i]=T;
        ts_desordenado[timeslot_con_mas_examenes].len_examenes_en_timeslot = -10; //con esto lo "elimino". en esta linea dejo inutilizable ts_desordenado
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
    return timeslots_ord;
}

int printSolucion(TIMESLOT* sol, int cant_timeslots_HC, int tipo_solucion){
    printf("\n");
    if(tipo_solucion == 0){
        printf("---------------------- SOLUCION CON HC------------------------\n");
    }
    else{
        printf("---------------------- SOLUCION FINAL------------------------\n");
    }
    for(int s=0; s<cant_timeslots_HC; s++){
        printf("timeslot: %d\n", s);
        for(int o=0; o<sol[s].len_examenes_en_timeslot;o++){
            int examen = sol[s].examenes_en_timeslot[o];    
            printf("id examen %d\n", sol[s].examenes_en_timeslot[o]+1);  
        }
        printf("\n");
    }
    printf("cantidad de timeslots HC: %d\n", cant_timeslots_HC);
    return 0;
}

TIMESLOT* doRespaldo(TIMESLOT* timeslots_ord, int cant_timeslots){
    TIMESLOT* R = (TIMESLOT*)malloc(cant_timeslots*sizeof(TIMESLOT));
    for(int i=0; i<cant_timeslots; i++){
        R[i]=timeslots_ord[i];
    }
    return R;
}

int main(int argc, char ** argv){

    FILE *exm, *stu;
    exm = fopen(examenes, "r");
    stu = fopen(estudiantes, "r");

    if (!exm || !stu) {
        printf("No se han podido abrir los archivos\n");
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
    for(int s=0; s<cant_timeslots; s++){
        printf("timeslot: %d\n", s);
        for(int o=0; o<timeslots_array[s].len_examenes_en_timeslot;o++){
            int examen = timeslots_array[s].examenes_en_timeslot[o];    
            printf("id examen %d\n", timeslots_array[s].examenes_en_timeslot[o]+1);
        }
        printf("\n");
    }
    int cant_timeslots_greedy = cant_timeslots;
    printf("cantidad de timeslots greedy: %d\n", cant_timeslots_greedy);
    printf("\n");

    //----------------------- Hill Climbing----------------------------------------

    /*tomamos el timeslot con mas examenes agendados
    tomamos el examen mas conflictivo de ese timeslot y lo asigno en el primer timeslot que no tenga conflito

    el orden siempre es de mas a menos examenes agendados 
    tomo solo 1 examen por timeslot por pasada, hasta que la "matriz solucion" este vacia
    cambiar el examen mas conflictivo -> movimiento
    */
    /*
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
    }*/
    //esto es para printear timeslots ordenados
    /*printf("\n");
    printf("------------- timeslots ordenados ------------------------\n");
    for(int i=0; i<cant_timeslots; i++){
        printf("timeslot: %d\n", timeslots_ord[i].timeslot);
        printf("cantidad examenes: %d\n", timeslots_ord[i].len_examenes_en_timeslot);
        printf("\n");
    }*/

    //aqui empieza el algoritmo HC
    TIMESLOT* timeslots_ord = ordenar_timeslots(cant_timeslots, timeslots_array);
    TIMESLOT* respaldo_sol = doRespaldo(timeslots_ord,cant_timeslots);
    int respaldo_cant_timeslots_HC = cant_timeslots;
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
    int se_mejoro_solucion = 0; //flag que controla la intensificacion del algoritmo. Si no se encuentra mejor solucion, se activa el flag y se detiene el algoritmo
    //no_agregar_a_timeslot = 0;
    //no_se_anadio_a_ningun_timeslot = 0;
    while(se_mejoro_solucion==0){
        cant_timeslots_HC = 0;
        max_cant_conflictos = -1;
        cant_examenes_reubicados=0;
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
                    max_cant_conflictos = -1 ; 
                    //aqui tengo que eliminar e1  de examenes_en_timeslot de su timeslot actual
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
                            for(int h=0; h<len_examenes_timeslot; h++){//recorro los examenes actuales
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
        printSolucion(sol, cant_timeslots_HC, 0);
        if(cant_timeslots_HC >= cant_timeslots){
            printf("No se mejoro solucion\n");
            sol = respaldo_sol;
            cant_timeslots_HC = respaldo_cant_timeslots_HC; 
            se_mejoro_solucion=1;
        }
        else{
            printf("Si se mejoro solucion\n");
            respaldo_sol = doRespaldo(sol,cant_timeslots_HC);
            respaldo_cant_timeslots_HC = cant_timeslots_HC;
            timeslots_ord = ordenar_timeslots(cant_timeslots_HC, sol);
            cant_timeslots = cant_timeslots_HC;
            sol = (TIMESLOT*)realloc(sol, sizeof(TIMESLOT));
        }
    }

    printSolucion(sol, cant_timeslots_HC, 1);
    
    printf("\n");
    printf("cantidad Final de timeslots HC: %d\n", cant_timeslots_HC);
    printf("cantidad de timeslots greedy: %d\n", cant_timeslots_greedy);



    fclose(exm);
    fclose(stu);
    return 0;
}