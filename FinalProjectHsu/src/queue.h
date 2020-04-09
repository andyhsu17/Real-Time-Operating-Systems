/*
 * queue.h
 *
 *  Created on: Feb 20, 2020
 *      Author: Andy Hsu
 */

#include <stdio.h>
#include <stdlib.h>

#ifndef SRC_QUEUE_H_
#define SRC_QUEUE_H_

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Structure which holds the Queue's Node information
//----------------------------------------------------------------------------------------------------------------------------------
struct node_t {
    // Message information
    struct info_t* msg;

    // Pointer to the next node in the queue
    struct node_t* next;
};

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Structure which holds the Message information
//----------------------------------------------------------------------------------------------------------------------------------
struct info_t {

    // Status of Button 2
    int btn0status;
    int btn1status;

    // Status of Button 1
    int capLeft;
	int capRight;
};

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief ENUM that holds button and cap sense flags/status
//----------------------------------------------------------------------------------------------------------------------------------
enum {
	BTN0_PRESSED 	= (1u << 0),
	BTN1_PRESSED	= (1u << 1),
	CAP_RIGHT		= (1u << 2),
	CAP_LEFT		= (1u << 3),
};

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Creates a queue.
///
/// @param[in] msg The message information
/// @param[in] size The size of the message array
///
/// @return the head of the new queue
//----------------------------------------------------------------------------------------------------------------------------------
struct node_t* create_queue(struct info_t* msg, int size);

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Create a new node for the queue
///
/// @param msg The message information
///
/// @return a newly allocated message
//----------------------------------------------------------------------------------------------------------------------------------
struct node_t* create_new_node(struct info_t* msg);

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Returns the top node in the queue
///
/// @param head The head of the queue
///
/// @return the message at the top of the queue
//----------------------------------------------------------------------------------------------------------------------------------
struct info_t* peek(struct node_t** head);

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Removes the element at the top of the queue.
///
/// @param head The head of the queue.
//----------------------------------------------------------------------------------------------------------------------------------
void pop(struct node_t** head);

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Push a new message into the queue
///
/// @param head The head of the queue
/// @param msg The message to be put into the queue
//----------------------------------------------------------------------------------------------------------------------------------
void push(struct node_t** head, struct info_t* msg);

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Determines whether the specified head is empty.
///
/// @param head The head of the Queue
///
/// @return True if the specified head is empty, False otherwise.
//----------------------------------------------------------------------------------------------------------------------------------
int is_empty(struct node_t** head);

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Remove all items from the queue
///
/// @param head The head of the queue
//----------------------------------------------------------------------------------------------------------------------------------
void empty_queue(struct node_t** head);

#endif /* SRC_QUEUE_H_ */
