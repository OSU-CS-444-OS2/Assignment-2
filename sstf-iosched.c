#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>


//struct		X
struct sstf_data {
	//Driver struct
	struct list_head queue;

	//add additional stuff
	//sector_t head;

	//Add direction for LOOK
	//int direction;
}; 

//request merge
//it makes it a link list... removes head and attackes the tail to new front
static void sstf_merged_requests( struct request_queue *q, struct request *rq, struct request *next ) {
	list_del_init( &next->queuelist );
}

//dispatch		
//Actaul moving to the disk
//return 0 if it doesn't work
//return 1 if it does work
static int sstf_dispatch( struct request_queue *q, int force ) {
	struct sstf_data *sd = q->elevator->elevator_data;

	if (!list_empty(&nd->queue)) {
		struct request *rq;
		rq = list_entry( nd->queue.next, struct request, queuelist );
		list_del_init( &rq->queuelist );
		elv_dispatch_sort( q, rq );
		return 1;
	}

	return 0;
}

//--------------------------------------------------------------------------------------------------
//---------------------------------------redo function----------------------------------------------
//--------------------------------------------------------------------------------------------------

//add request		X
//adds the new request to the tail of the list -> linux version
// new request to the correct location in the list -> is what we have to do
static void sstf_add_request( struct request_queue *q, struct request *rq ) {
	struct sstf_data *sd = q->elevator->elevator_data;

	//this should only happen if this is the starting of the queue
	//if(list has none){
	if( list_empty( sd->queue ) ){
		list_add( &rq->queuelist, &sd->queue );

	} else {  //list has been started

		//	Find where the request needs to be in the queue
		struct request rpNext, rpPrev; 
		rqCheck = list_entry( nd->queue.next, struct request, queuelist );
	
		//Don't know if this will work
		while( blk_rq_pos( rq ) < blk_rq_pos( rqCheck ) ){
			rqCheck = list_entry( rqCheck->queue.next, struct request, queuelist );
		}

		//	Put it into the list at the place
		list_add_tail( &rq->queuelist, &rqCheck->queuelist );
	}
}

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

//former request
//adding another queue to the list
static struct request * sstf_former_request( struct request_queue *q, struct request *rq ) {
	struct sstf_data *sd = q->elevator->elevator_data;

	if ( rq->queuelist.prev == &sd->queue ){
		return NULL;
	}

	return list_entry( rq->queuelist.prev, struct request, queuelist );
}

//latter request
static struct request * sstf_latter_request( struct request_queue *q, struct request *rq ) {
	struct sstf_data *sd = q->elevator->elevator_data;

	if ( rq->queuelist.next == &sd->queue ){
		return NULL;
	}

	return list_entry( rq->queuelist.next, struct request, queuelist );
}

//init queue
static int sstf_init_queue( struct request_queue *q, struct elevator_type *e ){
	struct sstf_data *sd;
	struct elevator_queue *eq;

	eq = elevator_alloc( q, e );
	if (!eq){
		return -ENOMEM;
	}

	ss = kmalloc_node( sizeof( *sd ), GFP_KERNEL, q->node );
	if ( !sd ) {
		kobject_put( &eq->kobj );
		return -ENOMEM;
	}

	eq->elevator_data = sd;

	INIT_LIST_HEAD( &sd->queue );

	spin_lock_irq( q->queue_lock ) ;
	q->elevator = eq;
	spin_unlock_irq( q->queue_lock );
	return 0;
}

//exit queue
static void sstf_exit_queue( struct elevator_queue *e ) {
	struct sstf_data *sd = e->elevator_data;

	BUG_ON( !list_empty( &sd->queue ) );
	kfree( sd );
}

//elevator
static struct elevator_type elevator_sstf = {
	.ops = {
		.elevator_merge_req_fn		= sstf_merged_requests,
		.elevator_dispatch_fn		= sstf_dispatch,
		.elevator_add_req_fn		= sstf_add_request,
		.elevator_former_req_fn		= sstf_former_request,
		.elevator_latter_req_fn		= sstf_latter_request,
		.elevator_init_f		= sstf_init_queue,
		.elevator_exit_fn		= sstf_exit_queue,
	},
	.elevator_name = "sstf",
	.elevator_owner = THIS_MODULE,
};

//init
static int __init sstf_init( void ) {
	return elv_register( &sstf_elevator );
}

//exit
static void __exit sstf_exit( void )  {
	elv_unregister( &sstf_elevator );
}

//call init
module_init( sstf_init );
//call exit
module_exit( sstf_exit );

//Extra stuff
MODULE_AUTHOR( "Kaiyuan Fan, Sophia Lui, Trevor Spear" );
MODULE_LICENSE( "GPL" );
MODULE_DESCRIPTION( "sstf IO scheduler" );
