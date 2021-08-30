#include <linux/init.h>
#include <linux/module.h> 
#include <linux/sched.h> 
#include <linux/rcupdate.h>
#include <linux/fdtable.h>
#include <linux/fs.h> 
#include <linux/fs_struct.h>
#include <linux/dcache.h>
#include <linux/slab.h>
#include <linux/net.h>
#include <net/sock.h>


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Lorenzo Susini");


static void kmonitor(void) 
{
	struct task_struct *task; 
	struct files_struct *current_files; 
	struct fdtable *files_table;
	struct path files_path;
	struct file *open_file;
	struct socket *socket;
	struct sock *sock;
	char *tmp_page;	
	char *cwd;
	int i;

	tmp_page = (char*)__get_free_page(GFP_KERNEL);

	for_each_process(task){
		
		printk("[%d]\t%s\n", task->pid, task->comm);

		current_files = task->files;
		files_table = files_fdtable(current_files);
		i = 0;

		while(files_table->fd[i] != NULL) { 

			open_file = files_table->fd[i];
			files_path = open_file->f_path;

			/* check if open_file refers to a socket */
			if(S_ISSOCK(file_inode(open_file)->i_mode)){

				socket = (struct socket *)open_file->private_data;
				sock = socket->sk;

				printk(
					"\t\tfd %d\tsocket," 
					"saddr %pI4," 
					"sport %u\n", 
					i,
					&sock->sk_rcv_saddr, 
					(unsigned int)sock->sk_num
				);
			}

			/* all other files */
			else {
    			cwd = d_path(&files_path, tmp_page, PAGE_SIZE);
				printk("\t\tfd %d\t%s\n", i, cwd);
			}

			i++;
		}

	}

    free_page((unsigned long)tmp_page);

}


static int kmonitor_init(void)
{ 
	printk(KERN_INFO "kmonitor inserted\n");
	kmonitor();
	return 0; 
} 


static void kmonitor_exit(void) 
{ 
 	printk(KERN_INFO "kmonitor removed\n"); 
} 


module_init(kmonitor_init);
module_exit(kmonitor_exit);
