#INCLUDE <16F887.h>
#DEVICE ADC=10
#use delay(clock=4000000,crystal)
#FUSES XT,NOPROTECT,NOWDT,NOBROWNOUT,PUT,NOLVP

#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7, parity=N, stop=1)

#BYTE PORTA= 5
#byte PORTB= 6
#byte PORTC= 7

long contador=0;
int16 duty=0;
int Timer2, Poscaler;
double RPM;
int tiempo_transcurrido = 0;

#INT_EXT //Conteo de pulsos del encoder en RB0
ext_isr()
{
    contador++;
}

#INT_TIMER1 // 
timer1_isr()
{
    tiempo_transcurrido++;
}

void main()
{
    // Configura los Puertos del PIC
    set_tris_a(0x01);
    set_tris_b(0x01); // RB0 como entrada
    

    // Configura la Entrada Analoga
    setup_adc_ports(sAN0); //Configuración de entrada analogica
    setup_adc(adc_clock_internal);

    // Configura el PWM
    Timer2=249; //Genera una onda cuadrada de 1khz
    Poscaler=1;
    setup_timer_2(t2_div_by_4, Timer2, Poscaler);
    setup_ccp1(ccp_pwm);//Operación en modo PWM

    // Configura el Timer1 para medir el tiempo transcurrido
    setup_timer_1(T1_INTERNAL | T1_DIV_BY_1);
    enable_interrupts(INT_TIMER1);// Activacion del Timer1 por desbordamiento

    // Habilita las Interrupciones para la lectura de los pulsos
    enable_interrupts(int_ext);
    ext_int_edge(L_TO_H);
    enable_interrupts(global);

    // Inicializa la comunicación serial
    printf("Iniciando...\r\n");

    while(1)
    {
        // Lee el Potenciometro
        set_adc_channel(0);
        delay_us(100);
        duty = read_adc(); //Lectura de potenciometro
        set_pwm1_duty(duty); //Establece el ciclo de trabajo PMW

        // Espera por 1 segundo
        delay_ms(999);

        // Después de 1 segundo, calcula las RPM
        RPM = (contador * 60) / 20;

        // Muestra la Velocidad en el puerto serial
        printf("RPM: %f\r\n", RPM);

        // Reinicia el contador de Pulsos
        contador = 0;

        // Reinicia el tiempo transcurrido
        tiempo_transcurrido = 0;
        set_timer1(0);
    }
}

