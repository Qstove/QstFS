/*
 *
 *  Created on: 02.04.2018
 *      Author: KustovAV
 */

#include "flash.h"
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <mem/mem.h>
#include <uart/uart.h>
#include <buf/buf.h>
#include <timer/timer.h>
#include <crc/crc32.h>
#include "qstfs_v1.1.h"
#include "md5.h"
#define OTL_PRINT 1
extern mem_pool_t pmr_pool;

void return_handler(uint32_t result)
{
	switch(result)
	{
	case 0:
		debug_printf("\tУспешно!\n");
		break;
	case 1:
		debug_printf("\nФайл с таким именем уже существует!");
		break;
	case 2:
		debug_printf("\nОшибочные параметры!");
		break;
	case 3:
		debug_printf("\nОшибка сохранности!");
		break;
	case 4:
		debug_printf("\nОшибка при записи файла!");
		break;
	case 5:
		debug_printf("\nФайл не найден!");
		break;
	case 6:
		debug_printf("\nДефрагментация не требуется!");
		break;
	case 7:
		debug_printf("\nПамять из пула не выделилась!");
		break;
	case 8:
		debug_printf("\nПроблема с флеш. Память не очистилась.");
		break;
	case 9:
		debug_printf("\nРаздел SSFI переполнен. (файлов больше чем 512)");
		break;
	case 10:
		debug_printf("\nНарушение сохранности файлов, CRC не совпало.");
		break;
	case 11:
		debug_printf("\nНет места на Flash памяти.");
		break;
	}
}

/*******************************************************************************
* Имя функции	 : init_qstfs.
* Описание	     : Подготовка памяти к работе с файловой системой.
* Аргументы      : Ничего.
* Возвращает     : 	OK 				- успех,
 					ERASE_SEC_ERR	- ошибка стирания сектора.
*******************************************************************************/
QST_RES init_qstfs(void)
{
	uint32_t sec_num;
	uint32_t result = 0;
	for (sec_num = 8; sec_num<32; sec_num++) result +=flash_sector_erase(sec_num);
	if(result)  return ERASE_SEC_ERR;
	else 		return OK;
}



/*******************************************************************************
* Имя функции	 : print_SSM.
* Описание	     : Распечатка SSM.
* Аргументы      : Ничего.
* Возвращает     : Ничего.
*******************************************************************************/
QST_RES print_SSM(void)
{
	sys_sec_fileinfo * ssfi = (sys_sec_fileinfo *)SYS_SEC_FI_ADDR;
	filehead * fh_analise;
	uint32_t i, BytesUsed = 0, space_available = 0;
	for(i=0;i<512;i++)
	{
		if(*(ssfi+i)->name == 0xff)
		{
			if(i == 0)	BytesUsed = 0;
			else
			{
				fh_analise = (filehead*)((ssfi+i)-1 )->adress;
				BytesUsed = (uint32_t)fh_analise+sizeof(filehead)+fh_analise->data_size - (uint32_t)FLASH_BASE_ADDR_qstFS;
			}
			break;
		}
		else if(i == 511) return SSFI_FULL;
	}
	space_available = (SEC_SIZE*SECTOR_COUNT) - BytesUsed;
	debug_printf("\n__________________Состояние памяти, байты___________________");
	debug_printf("\nВсего     %u (hex: %#x) байт флеш памяти, из них", (SEC_SIZE*SECTOR_COUNT), (SEC_SIZE*SECTOR_COUNT));
	debug_printf("\nЗанято:   %u (hex: %#x) байт флеш памяти.", BytesUsed, BytesUsed);
	debug_printf("\nCвободно: %u (hex: %#x) байт флеш памяти.\n", space_available, space_available);
	debug_printf("\n__________________Состояние памяти, Кб___________________");
	debug_printf("\nВсего     %u  Кб флеш памяти, из них", (SEC_SIZE*SECTOR_COUNT)/1024);
	debug_printf("\nЗанято:   %u  Кб флеш памяти.", BytesUsed/1024);
	debug_printf("\nCвободно: %u  Кб флеш памяти.\n", space_available/1024);
	return OK;
}



/*******************************************************************************
* Имя функции	 : adress_analyser.
* Описание	     : Анализирует адрес, возвращая сектор, в котором эти данные лежат.
* Аргументы      : Адрес.
* Возвращает     : Номер сектора.
*******************************************************************************/
static
uint32_t adress_analyser(uint32_t * adress)
{
	uint32_t sec_num = 666;
	if (adress < (uint32_t*)SEC32_START) sec_num = 31; //31 сектор
	if (adress < (uint32_t*)SEC31_START) sec_num = 30;
	if (adress < (uint32_t*)SEC30_START) sec_num = 29;
	if (adress < (uint32_t*)SEC29_START) sec_num = 28;
	if (adress < (uint32_t*)SEC28_START) sec_num = 27;
	if (adress < (uint32_t*)SEC27_START) sec_num = 26;
	if (adress < (uint32_t*)SEC26_START) sec_num = 25;
	if (adress < (uint32_t*)SEC25_START) sec_num = 24;
	if (adress < (uint32_t*)SEC24_START) sec_num = 23;
	if (adress < (uint32_t*)SEC23_START) sec_num = 22;
	if (adress < (uint32_t*)SEC22_START) sec_num = 21;
	if (adress < (uint32_t*)SEC21_START) sec_num = 20;
	if (adress < (uint32_t*)SEC20_START) sec_num = 19;
	if (adress < (uint32_t*)SEC19_START) sec_num = 18;
	if (adress < (uint32_t*)SEC18_START) sec_num = 17;
	if (adress < (uint32_t*)SEC17_START) sec_num = 16;
	if (adress < (uint32_t*)SEC16_START) sec_num = 15;
	if (adress < (uint32_t*)SEC15_START) sec_num = 14;
	if (adress < (uint32_t*)SEC14_START) sec_num = 13;
	if (adress < (uint32_t*)SEC13_START) sec_num = 12;
	if (adress < (uint32_t*)SEC12_START) sec_num = 11;
	if (adress < (uint32_t*)SEC11_START) sec_num = 10;
	if (adress < (uint32_t*)SEC10_START) sec_num = 9;
	if (adress < (uint32_t*)SEC9_START)  sec_num = 8;
	/*
	 * ЗДЕСЬ ПОКОИТСЯ ОБРАЗ
	 *
	if (adress < (uint32_t*)SEC8_START)  sec_num = 7;
	if (adress < (uint32_t*)SEC7_START)  sec_num = 6;
	if (adress < (uint32_t*)SEC6_START)  sec_num = 5;
	if (adress < (uint32_t*)SEC5_START)  sec_num = 4;
	if (adress < (uint32_t*)SEC4_START)  sec_num = 3;
	if (adress < (uint32_t*)SEC3_START)  sec_num = 2;
	if (adress < (uint32_t*)SEC2_START)  sec_num = 1;
	if (adress < (uint32_t*)SEC1_START)  sec_num = 0;	//0 сектор
	*/

#if OTL_PRINT
	debug_printf("\nАнализ входящего адреса %#x. Принадлженость %u сектору.", adress, sec_num);
#endif
	return sec_num;
}


/*******************************************************************************
* Имя функции	 : create_file.
* Описание	     : Функция создание файла.
* Аргументы      : Указатель на имя файла(8б),
 				   размер данных в байтах,
 				   указатель на данные.
* Возвращает     : OK 				 - успех.
 				   MEM_ALLOC_ERR	 - память из пула не выделилась.
 				   NOT_SAVED 		 - ошибка сохранности.
 				   INVALID_PARAM 	 - ошибочные параметры.
 				   NAME_ALREADY_USED - файл с таким именем уже существует.
*******************************************************************************/
QST_RES create_file(uint8_t * filename, uint32_t size, void * data)
{
	filehead filehead_t;													//локальная голова файла.
	filehead * fh = &filehead_t;											//указатель на локальную голову файла.
	filehead * fh_analise = 0;												//указатель анализа головы.
	sys_sec_fileinfo fileinfo_t;											//локальная ячейка SSFI.
	sys_sec_fileinfo * fi = &fileinfo_t;									//указатель на локальную ячейку SSFI.
	sys_sec_fileinfo * ssfi = (sys_sec_fileinfo *)SYS_SEC_FI_ADDR;			//указатель на SSFI(FLASH), информирование о записанных файлах
	uint32_t i = 0, j = 0;
	uint32_t * ptr32_burn_ssfi = 0;
	uint32_t * burn_to32data = 0;
	uint32_t * burn_to32 = 0;												//INT  указатель на область данных, куда будем писать файл. ДЛЯ ЗАПИСИ.
	uint8_t  * burn_to8  = 0;												//CHAR указатель на область данных, куда будем писать файл. ДЛЯ МАНИПУЛЯЦИЙ С АДРЕСАМИ.


	/****************************************
	 * Проверка имени файла на уникальность *
	 ****************************************/
	for(i=0; i<512; i++)	if(!memcmp((ssfi+i)->name, filename,8))	/*return NAME_ALREADY_USED*/delete_file(filename);	//изначально было исключительной ситуации, однако потом изменил на перезапись.
	if (size <= 0) 													return INVALID_PARAM;


	/*******************************
	 * Инициализация головы файла. *
	 *******************************/
	memcpy(fh->name, filename, 8);							//копирование имени файла в структуру
	fh->data_size = size;									//заполнения поля размер данных, в байтах
	fh->CRC = crc32(data,size);
#if	OTL_PRINT
	debug_printf("\nИмя записываемого файла: %s",fh->name);
	debug_printf("\nРазмер записываемого файла: %#x байт", 	fh->data_size+sizeof(filehead));
	debug_printf("\nКонтрольная сумма: %#x\n", 	fh->CRC);
#endif


	/***********************
	 * Запись ячейки SSFI. *
	 ***********************/
	for(i=0;i<512;i++)
	{
#if	OTL_PRINT
		debug_printf("\n*(ssfi+i)->name, %#x,",  *(ssfi+i)->name);
#endif
		if(*(ssfi+i)->name == 0xff)
		{
			if(i == 0)
			{
				burn_to8  = (uint8_t*)FLASH_BASE_ADDR_qstFS;				//откуда будем писать.
				burn_to32 = (uint32_t*)burn_to8;
#if	OTL_PRINT
				debug_printf("Первая запись на флеш! Начнем прямо с %#x\n", burn_to32);
#endif
			}
			else
			{
#if	OTL_PRINT
				debug_printf("Последний записанный файл:\nИмя: %s\nадрес головы: %#x\n", (ssfi+i-1)->name, (ssfi+i-1)->adress);	//указатель информацию по последнему записанному файлу
				debug_printf("Записывать будем %u файл по счету\n", i);
#endif
				fh_analise 	= (filehead*)(ssfi+i-1)->adress;
				burn_to8  	= (uint8_t*)fh_analise+sizeof(filehead)+fh_analise->data_size;						//откуда будем писать ФАЙЛ.
				if(((uint32_t)burn_to8+size)>SYS_SEC_FI_ADDR) return MEM_FULL;											//память заполнена
			}
			memcpy(fi->name, fh->name, 8);
			fi->adress = (uint32_t*)burn_to8;
			ptr32_burn_ssfi = (uint32_t *)fi;
			burn_to32 = (uint32_t*)(ssfi+i);
			for(j = 0; j < 3; j++) if(flash_write_word(burn_to32+j, *(ptr32_burn_ssfi+j))) return BURNING_ERROR;							//запись новой ячейки SSFI
			burn_to32 = (uint32_t*)burn_to8;
			break;
		}
		else if(i == 511) return SSFI_FULL;
	}
#if	OTL_PRINT
	debug_printf("\nЗапись Файла размером %#x", fh->data_size+sizeof(filehead));
	debug_printf("\nПишем по адресу %#x", burn_to32);
#endif
	/************************************
	 * 1. Запись головы.				*
	 * 2. Запись данных.				*
	 * 3. Проверка записанных данных.	*
	 ************************************/

	for (i = 0; i < 4; i++) 		flash_write_word((burn_to32+i), *((uint32_t*)fh+i));						//1.
	burn_to32data = burn_to32+i;
	for	(i = 0; i < (size/4); i++)	flash_write_word((burn_to32data+i), *((uint32_t*)data+i));					//2.
	for	(i = 0; i < (size/4); i++)																				//3
	{
		if ( *(burn_to32data+i) != *((uint32_t*)data+i) )
		{
			debug_printf("\nЗаписали %#x по адресу %#x, а должно быть %#x", *(burn_to32data+i), (burn_to32data+i), *((uint32_t*)data+i) );
			return BURNING_ERROR;
		}
	}
#if	OTL_PRINT
		debug_printf("\nЗапись окончена!");
#endif
	return OK;
}



/*******************************************************************************
* Имя функции	 : files_on_flash.
* Описание	     : Выдает информацию о записанных файлах на флеш.
* Аргументы      : Тип. 0 - без печатей
 						1 - с печатями.
* Возвращает     : Общее количество записанных файлов.
*******************************************************************************/
uint32_t files_on_flash(uint32_t type)
{
	MD5_CTX ctx;
	uint8_t hash[16];
	memset(&hash, 0, 16);
	sys_sec_fileinfo * fi = (sys_sec_fileinfo*)SYS_SEC_FI_ADDR;
	filehead * fh_analise;
	uint32_t counter = 0, bad_counter = 0, i, k;

	for(i=0; i<512; i++)
	{
		if(*(uint32_t*)(fi+i) == 0xffffffff)
		{
			if(type)					debug_printf("\nНайдено %u файлов.\n",counter);
			if(bad_counter) if (type) 	debug_printf("\nИз них %u помечены как стертые.\n",bad_counter);
			break;
		}
		else if (*(uint32_t*)(fi+i)->name == 0)
		{
			if (type)					debug_printf("\nНуль-файл по адресу %#x\n", (fi+i)->adress);
			bad_counter++;
			counter++;
			continue;
		}
		else
		{
			memset(&hash, 0, 16);
			if (type)
			{
				debug_printf("\nФайл %s\nАдрес данных %#x(%#x)", (fi+i)->name, (fi+i)->adress+sizeof(filehead), (fi+i)->adress);
				fh_analise = (fi+i)->adress;
				debug_printf("\nРазмер: %u байт", fh_analise->data_size);
				debug_printf("\nCRC: %#x", fh_analise->CRC);

				//посчитать MD5.
				md5_init(&ctx);
				md5_update(&ctx, (uint8_t*)(fh_analise+1), fh_analise->data_size);
				md5_final(&ctx, &hash);

				//вывести
				debug_printf("\nMD5: ");
				for(k=0; k<16; k++)	debug_printf("%x", hash[k]);
				debug_printf("\n");
			}
			counter++;
		}
	}
	return counter;
}



/*******************************************************************************
* Имя функции	 : read_file.
* Описание	     : Функция чтения файла
* Аргументы      : Имя файла
* Возвращает     : Структуру(read_response), полями которой являются адрес и размер данных
*******************************************************************************/
read_response read_file(uint8_t * filename)	//возвращает структуру, полями которой являются указатель на данные и размер этих данных(байт)
{
/*************************************************************************************
 * Так как на входе имеется только имя файла, то имеет смысл прогнать поиск по ssfi, *
 * чтобы убедиться в существовании файла в памяти. Так как нам пришло имя файла,	 *
 * нужно проверить его наличие в нашей памяти. Прогоним поиск по ssfi				 *
 *************************************************************************************/
#if	OTL_PRINT
	debug_printf("\nФайл %s", filename);
#endif
	uint32_t i=0;
	read_response ret_struct;
	sys_sec_fileinfo * ssfi = (sys_sec_fileinfo *)SYS_SEC_FI_ADDR;
	while(memcmp((ssfi+i)->name, filename,8))
	{
		if(i>512)
		{
			ret_struct.data = 0;
			ret_struct.size = 0;
#if	OTL_PRINT
			debug_printf(" не найден!", i);
#endif
			return ret_struct;
		}
		i++;
	}
	debug_printf("найден!", i);
	filehead * fh = (filehead *)(ssfi+i)->adress;
	ret_struct.size = fh->data_size;
	ret_struct.data = (uint32_t *)(fh+1);
#if	OTL_PRINT
	debug_printf("\nАдрес на данные: %#x",  ret_struct.data);
	debug_printf("\nРазмер данных: %#x\n", 	ret_struct.size);
#endif
	return ret_struct;



}



/*******************************************************************************
* Имя функции	 : delete_file.
* Описание	     : Функция удаления (предварительное) файла
* Аргументы      : Имя файла
* Возвращает     : OK 			- успех
				   NOT_EXISTS 	- файл не найден
*******************************************************************************/
QST_RES delete_file(uint8_t * filename)
{
	/**
	 * 1. Проверка наличия файла.
	 * 2. Изменение его имени на "0".
	 **/
	uint32_t i=0;
	sys_sec_fileinfo * ssfi = (sys_sec_fileinfo*)SYS_SEC_FI_ADDR;
	while(memcmp((ssfi+i)->name, filename,8))	//1
	{
		if(i>512) return NOT_EXISTS;	//такого файла нет.
		i++;
	}
	uint32_t * ptr32_del_in_ssm = (uint32_t*)(ssfi+i);		//указатель для стирания половины имени в SSFI
	filehead * fh = (filehead*)(ssfi+i)->adress;			//указатель на голову стираемого файла
	uint32_t * ptr32_del_in_fh = (uint32_t*)fh;				//указатель для стирания  половины имени в голове фйла
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	flash_write_word(ptr32_del_in_ssm, 0);					//стерли первые 4 байта имени в SSFI
	ptr32_del_in_ssm++;
	flash_write_word(ptr32_del_in_ssm, 0);					//стерли вторые 4 байта имени в SSFI
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	flash_write_word(ptr32_del_in_fh, 0);					//стерли первые 4 байта имени в голове файла
	ptr32_del_in_fh++;
	flash_write_word(ptr32_del_in_fh, 0);					//стерли вторые 4 байта имени в голове файла
#if	OTL_PRINT
	debug_printf("\nФайл %s помечен как стертый.\n", filename);
#endif
	return OK;
}



/*******************************************************************************
* Имя функции	 : flash_defrag.
* Описание	     : Функция дефрагментации(избавление от нуль-файлов) flash памяти
* Аргументы      : ничего
* Возвращает     :  OK 				 - успех
* 					DEFRAG_NOT_NEED  - нет смысла в дефрагментации
* 					MEM_ALLOC_ERR  	 - пул не "дал" память
* 					BURNING_ERROR 	 - ошибка сохранности в ОЗУ.
*******************************************************************************/
QST_RES  flash_defrag(void)
{
/*******************************************
 * Механизм поиска смысла в дефрагментации *
 * И поиска сектора, с которого начать ее. *
 *******************************************/
	uint32_t offset = 0, offset_appendixREAL = 0, offset_appendixZERO = 0;	//размер файла в байтах, размер остатка действительного файла в интах, размер остатка нуль-файла в интах.
	uint32_t bad_counter = 0;
	uint32_t sec_num = 666;
	uint32_t i = 0;
	uint32_t * ptr_defrag_start;
	sys_sec_fileinfo * fi = (sys_sec_fileinfo*)SYS_SEC_FI_ADDR;
	for(i=0; i<512; i++)
	{
		if (*(uint32_t*)(fi+i)->name == 0)					//найден нульфайл, определим сектор, который будем дефрагментировать
		{
			bad_counter++;									//проверка в необходимости дефрагментации
			sec_num = adress_analyser((fi+i)->adress);
			ptr_defrag_start = (fi+i)->adress;				//запомнили адрес нульфайла
			offset_appendixREAL = ((uint32_t)ptr_defrag_start/*0xBC200000*/ - ((uint32_t)(FLASH_BASE_ADDR)/*0xBC000000*/+(uint32_t)FLASH_SECTOR_SIZE*(sec_num)))/*0x200000*//4;
			break;
		}
	}
	if(bad_counter == 0) return DEFRAG_NOT_NEED;


/***********************************************
 * Создание вспомогательных элементов, буферов *
 ***********************************************/
	uint32_t  j = 0, k = 0, wsb = 0, ssfi_b = 0;
	uint32_t  tumbler = 1;
	uint8_t  * saveptr8 = 0;
	uint32_t * saveptr32 = 0;
	uint32_t * save_burn_ptr = 0;
	uint32_t * burn_to = 0;
	filehead * fh_analise;
	sys_sec_fileinfo *	buff_ssfi 	= mem_alloc_dirty(&pmr_pool, 0x1800);		//буфер ssfi
	if(buff_ssfi == 0)	return MEM_ALLOC_ERR;
	uint32_t * 			buff_ws 	= mem_alloc_dirty(&pmr_pool, 0x40000);		//буфер WS data
	if(buff_ws == 0) 	return MEM_ALLOC_ERR;


/*********************************************
 * Механизм выполнения посекторного анализа. *
 *********************************************/
	save_burn_ptr = (uint32_t*)(FLASH_BASE_ADDR+(sec_num*FLASH_SECTOR_SIZE));
	memset(buff_ssfi, 0xff, 0x1800);
	for(; sec_num<31; sec_num++)
	{
		memset(buff_ws, 0xff, 0x40000);
		i=0;


		/*************************************************
		 * Вычисление указателя, откуда будем сохранять. *
		 * Определение аппендиксов.						 *
		 *************************************************/
		if(offset_appendixZERO)
		{
#if	OTL_PRINT
			debug_printf( "\nБыл нуль-файл аппендикс длинной %u интов.", offset_appendixZERO);
#endif
			saveptr8 = (uint8_t *)(FLASH_BASE_ADDR + (sec_num*FLASH_SECTOR_SIZE))+(offset_appendixZERO*4);	//если аппендикс был нуль-файла то просто смещаем указатель на его размер и кладем болт
		}
		else if(offset_appendixREAL)
		{
#if	OTL_PRINT
			debug_printf( "\nБыл файл аппендикс длинной %u интов.", offset_appendixREAL);
#endif
			saveptr32 = (uint32_t*)(FLASH_BASE_ADDR + (sec_num*FLASH_SECTOR_SIZE));
			for(j=0; j<offset_appendixREAL; j++)
			{
				*(buff_ws+wsb) = *(saveptr32+j);//запись в буфер озу аппендикса
				wsb++;
			}
			saveptr8 = (uint8_t *)(FLASH_BASE_ADDR + (sec_num*FLASH_SECTOR_SIZE))+(offset_appendixREAL*4);	//если аппендикс был действительного файла то записываем его в буфер и смещаем указатель на его размер
		}
		else saveptr8 = (uint8_t*)(FLASH_BASE_ADDR + (sec_num*FLASH_SECTOR_SIZE));
		fh_analise = (filehead*)saveptr8;
		if(*fh_analise->name == 0xff)			//проверка на наличие данных
		{
			if(offset_appendixREAL)				//если файлов нет на секторе и был апендикс, то запишем его, так как дальше - выход из цикла.
			{
#if	OTL_PRINT
			debug_printf("\nДопишем последний аппендикс на флеш по адресу %#x", burn_to);
#endif
				burn_to = save_burn_ptr;
				for(j = 0; j < offset_appendixREAL; j++)
				{
					flash_write_word(burn_to, *(saveptr32+j));
					burn_to++;
					save_burn_ptr = burn_to;
				}
			}
#if	OTL_PRINT
			debug_printf("\nФайлов на %u секторе не обнаружено.", sec_num);
#endif
			break;
		}
		offset_appendixREAL=0;
		offset_appendixZERO=0;
#if	OTL_PRINT
		debug_printf( "\nСохраняем Work data %u сектора.", sec_num);
#endif


		/***************************
		 * Сохраним рабочий сектор *
		 ***************************/
		while(tumbler)														//сохранение WS data на ОЗУ уже без нуль файлов.
		{
			fh_analise = (filehead*)(saveptr8+i);
#if	OTL_PRINT
debug_printf("\nАнализ по адресу: %#x", fh_analise);
#endif
			if(*(uint32_t*)fh_analise->name == 0)							//нуль файл обнаружен
			{
				offset = fh_analise->data_size+sizeof(filehead);
#if	OTL_PRINT
				debug_printf( "\nWD %u сектора: нуль-файл по адресу %#x, размером %u байт(hex:  %#x). Skiping...",sec_num, fh_analise, offset, offset);
#endif
				saveptr32 = (uint32_t*)(saveptr8+i);
				for(k = 0; k < offset/4; k++)								//чек того, что нуль-файл не выходит за пределы сектора.
				{
					if((uint32_t)(saveptr32+k) > (FLASH_BASE_ADDR + ((sec_num+1)*FLASH_SECTOR_SIZE)-1)) //максимальный адрес сектора
					{
						offset_appendixZERO = (offset/4)-k;					//сохраняем остаток файла.
#if	OTL_PRINT
						debug_printf( "\nВышибло из цикла сохранения WS DATA!(0) 564 строчка (saveptr32+k) = %#x, offset_appendixZERO = %u",(saveptr32+k),offset_appendixZERO );
#endif
						tumbler = 0;
						break;
					}
				}
				i+=offset;
			}
			else if(*(uint32_t*)fh_analise->name == 0xffffffff)				//данных больше нет.
			{
#if	OTL_PRINT
				debug_printf( "\nWD %u сектора: Конец области действительных данных %#x. Конец сохранения WS.",sec_num,fh_analise );
#endif
				tumbler = 0;
			}
			else															//данные действительны
			{
				offset = fh_analise->data_size + sizeof(filehead);
#if	OTL_PRINT
				debug_printf( "\nWD %u сектора: Найден файл  %s, по адресу %#x, размером %u байт(hex:  %#x), сохраняю.",sec_num, fh_analise->name, fh_analise,  offset, offset);
#endif
				saveptr32 = (uint32_t*)(saveptr8+i);
				for(j = 0; j < offset/4; j++)								//запись файла в ОЗУ в интах.
				{
					if((uint32_t)(saveptr32+j) > (FLASH_BASE_ADDR + ((sec_num+1)*FLASH_SECTOR_SIZE)-1)) //максимальный адрес сектора
					{
						offset_appendixREAL = (offset/4)-j;
#if	OTL_PRINT
						debug_printf( "\nВышибло из цикла сохранения WS DATA!(действ.) 581 строчка (saveptr32+j) = %#x, offset_appendixREAL = %u",(saveptr32+j), offset_appendixREAL );
#endif+
						tumbler = 0;
						break;
					}
					*(buff_ws+wsb) 	= *(saveptr32+j);
					if( *(buff_ws+wsb) != *(saveptr32+j) )
					{
						mem_free(buff_ssfi);
						mem_free(buff_ws);
						return BURNING_ERROR;		//ошибка сохранения в озу
					}
					wsb++;						//инкремент указателя на пул. //количество интов
				}
				i+=offset;
			}
		}
		tumbler = 1;
#if	OTL_PRINT
		debug_printf("\nWD %u сектора сохранно.", sec_num);
#endif
		burn_to = save_burn_ptr;


/**************************
*Запись WS data во флеш. *
**************************/
		i = k = j = 0;
		flash_sector_erase( adress_analyser(FLASH_BASE_ADDR + (sec_num*FLASH_SECTOR_SIZE)) );
#if	OTL_PRINT
		debug_printf("\nЗапись WD %u сектора.", sec_num);
		debug_printf("\nwsb = %u", wsb);
		debug_printf("\nЗаписываем по адресу %#x...", burn_to);
#endif
		for(j = 0; j < wsb; j++)
		{

			flash_write_word(burn_to, *(buff_ws+j));						//запись файла
			burn_to++;
		}
		save_burn_ptr = burn_to;											//куда писать следующий сектор
		wsb=0;
	}


/***************************************
 * Формирование SSFI и запись во флеш. *
 ***************************************/
	flash_sector_erase( adress_analyser(SYS_SEC_FI_ADDR) );
#if	OTL_PRINT
	debug_printf("\nЗапись SSFI.");
#endif
	fh_analise = (filehead*)FLASH_BASE_ADDR_qstFS;
	for(ssfi_b = 0; ssfi_b < 512; ssfi_b++)
	{
		if(*fh_analise->name == 0xff) break;
		memcpy((buff_ssfi+ssfi_b)->name, fh_analise->name,8);
		(buff_ssfi+ssfi_b)->adress = (uint32_t*)fh_analise;
		offset = fh_analise->data_size + sizeof(filehead);
		fh_analise = (filehead*)((uint8_t*)fh_analise + offset);
	}
	burn_to = (uint32_t*)SYS_SEC_FI_ADDR;
	saveptr32 = (uint32_t*)buff_ssfi;
	for(i=0; i<( (ssfi_b*sizeof(sys_sec_fileinfo) )/4); i++)
	{
		flash_write_word(burn_to, *(saveptr32+i));
		burn_to++;
	}
	mem_free(buff_ws);
	mem_free(buff_ssfi);
#if	OTL_PRINT
	debug_printf("\nУспех!");
#endif
	return OK;
}


/*******************************************************************************
* Имя функции	 : CRC_checker.
* Описание	     : функция проверки сохранности данных.
* Аргументы      : Тип. 0 - без печатей
 						1 - с печатями.
* Возвращает     : 	OK 		- успех,
 					CRC_ERR	- ошибка стирания сектора.
*******************************************************************************/
QST_RES CRC_check(uint32_t type)
{
	uint32_t i;
	uint8_t * CRC_ptr = 0;
	filehead * fh_analise;
	uint32_t bad_CRC_flag = 0;
	sys_sec_fileinfo * fi = (sys_sec_fileinfo*)SYS_SEC_FI_ADDR;
	for(i=0; i<512; i++)
	{
		if(*(fi+i)->name == 0xff)		break;		//если файлов больше нет.
		else if (*(fi+i)->name == 0)	continue;	//если файл стертый
		else										//если файл действительный
		{
			fh_analise = (filehead*)(fi+i)->adress;
			CRC_ptr = (uint8_t*)fh_analise+sizeof(filehead);
			if(fh_analise->CRC != crc32(CRC_ptr, fh_analise->data_size))
			{
				if (type)				debug_printf("\nФайл %s НЕСОХРАНЕН или ПОВРЕЖДЕН! Старая CRC: %#x  Новая CRC: %#x\n", (fh_analise)->name, (fh_analise)->CRC, crc32(CRC_ptr,fh_analise->data_size));
				bad_CRC_flag = 1;
			}
#if OTL_PRINT
		debug_printf("\nФайл %s. Записанное CRC: %#x\nВычисленный CRC: %#x\n", (fh_analise)->name, (fh_analise)->CRC, crc32(CRC_ptr,fh_analise->data_size));
#endif
		}
	}
	if (bad_CRC_flag) return CRC_ERR;
	else			  return OK;
}
