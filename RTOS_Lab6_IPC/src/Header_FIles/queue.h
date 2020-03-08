/*
 * queue.h
 *
 *  Created on: Feb 20, 2020
 *      Author: Andy Hsu
 */
#include "main.h"

#ifndef SRC_HEADER_FILES_QUEUE_H_
#define SRC_HEADER_FILES_QUEUE_H_

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Structure which holds the Queue's Node information
//----------------------------------------------------------------------------------------------------------------------------------
struct node_t {
    // Message information
    struct info_t * message;

    // Pointer to the next node in the queue
    struct node_t* next;
};

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Structure which holds the Message information
//----------------------------------------------------------------------------------------------------------------------------------
struct info_t {

    // Button 0 & button 1 Flag
    bool button0Status;
    bool button1Status;

    // Capslide status
    uint32_t capStatus;
};

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Enumeration which holds the possible event flag values
//----------------------------------------------------------------------------------------------------------------------------------
enum{
	BUT0_PRESSED 	= (1u << 0),
	BUT1_PRESSED 	= (1u << 1),
	CAP_LEFT 		= (1u << 2),
	CAP_RIGHT		= (1u << 3),
};

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Creates a queue.
///
/// @param[in] message The Message information
/// @param[in] size The size of the task array
///
/// @return the head of the new queue
//----------------------------------------------------------------------------------------------------------------------------------
struct node_t* create_queue(struct info_t * message, int size);

//----------------------------------------------------------------------------------------------------------------------------------
/// @brief Create a new node for the queue
///
/// @param task The message information
///
/// @return a newly allocated message
//----------------------------------------------------------------------------------------------------------------------------------
struct node_t* create_new_node(struct info_t * message);

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
/// @param message The message to be put into the queue
//----------------------------------------------------------------------------------------------------------------------------------
void push(struct node_t** head, struct info_t* message);

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

#endif /* SRC_HEADER_FILES_QUEUE_H_ */
