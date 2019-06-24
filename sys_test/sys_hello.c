#include <linux/module.h>//与module相关的信息
 
#include <linux/kernel.h>
#include <linux/init.h>      //与init相关的函数

#include <linux/slab.h> //kfree
#include <linux/sysfs.h> //_ATTR_RW

#define debug_print(fmt, ...) printk(KERN_INFO fmt, ##__VA_ARGS__)
#if 0
#define __ATTR_RW_FREE(_name) __ATTR(_name, 0666, _name##_show, _name##_store)

#define __ATTR(_name, _mode, _show, _store) {				\
	.attr = {.name = __stringify(_name),				\
		 .mode = VERIFY_OCTAL_PERMISSIONS(_mode) },		\
	.show	= _show,						\
	.store	= _store,						\
}
#endif


/*struct type define*/
struct my_attribute
{
    struct attribute attr;
    
    ssize_t (*show)(struct kobject *kobj, struct my_attribute *attr,
        const char *buf);
    ssize_t (*store)(struct kobject *kobj, struct my_attribute *attr,
        const char *buf, size_t count);
};


static ssize_t myspec_attr_show(struct kobject *kobj, struct my_attribute *attr,
        const char *buf)
{
    debug_print(KERN_INFO "myspec attr show kobj->name = %s, attr->name = %s, *buf = %s\n",
        kobj->name, attr->attr.name, buf);
    
    return 0;
};
static ssize_t myspec_attr_store(struct kobject *kobj, struct my_attribute *attr,
        const char *buf, size_t count)
{
   debug_print(KERN_INFO "myspec attr store kobj->name = %s, attr->name = %s, *buf = %s, count = %d\n",
        kobj->name, attr->attr.name, buf, (int)count);
    
    return count;

};

//static struct my_attribute myspec_attr = __ATTR_RW(myspec_attr);
static struct my_attribute myspec_attr = {
    .attr = {
        .name = "test_sys",
        .mode = 0666,
    },
    .show = myspec_attr_show,
    .store = myspec_attr_store,
};






/*kobj_type sysfs_ops*/
static ssize_t my_attr_show(struct kobject *kobj, struct attribute *attr,
			      char *buf)
{
	ssize_t ret = 0;
	struct my_attribute *m_attr;

    debug_print(KERN_INFO "sysfs ops show kobj->name = %s, attr->name = %s, *buf = %s\n", kobj->name, attr->name, buf);
	m_attr = container_of(attr, struct my_attribute, attr);
	if (m_attr->show)
		ret = m_attr->show(kobj, m_attr, buf);
    return ret;
}

/*kobj_type sysfs_ops*/
static ssize_t my_attr_store(struct kobject *kobj, struct attribute *attr,
			       const char *buf, size_t count)
{
	ssize_t ret = 0;
	struct my_attribute *m_attr;

    debug_print(KERN_INFO "sysfs ops store kobj->name = %s, attr->name = %s, *buf = %s\n", kobj->name, attr->name, buf);
	m_attr = container_of(attr, struct my_attribute, attr);
	if (m_attr->store)
		ret = m_attr->store(kobj, m_attr, buf, count);
    return count;
}


const struct sysfs_ops my_sysfs_ops = {
    .show	= my_attr_show,
	.store	= my_attr_store,
};

static void new_kobj_release(struct kobject *kobj)
{
	debug_print(KERN_INFO "release kobject: (%p), name: %s: %s\n", kobj, kobj->name, __func__);
	kfree(kobj);
    return;
}



static struct attribute *my_default_attrs[] = {
    &myspec_attr.attr,
    NULL,
};


/*kobj_type*/
static struct kobj_type new_kobj_ktype = {
	.release	= new_kobj_release,
	.sysfs_ops	= &my_sysfs_ops,
    .default_attrs = my_default_attrs,
};






static struct kobject *new_kobj = NULL;




static int create_new_kobject(void)
{   
    int ret = 0;

    new_kobj = kzalloc(sizeof(struct kobject), GFP_KERNEL);
    ret = kobject_init_and_add(new_kobj, &new_kobj_ktype, NULL, "test");
    debug_print(KERN_INFO "create kobject, error code[%d]\n", ret);
    return 0;
}
static int kill_new_kobject(void)
{   
    debug_print(KERN_INFO "kill kobject\n");
    if(new_kobj)
    {
        kobject_del(new_kobj);
    }
    return 0;
}
static int __init hellokernel_init(void)
{
        debug_print(KERN_INFO "Hello kernel sys_fs!\n");
        create_new_kobject();
        return 0;
}
 
static void __exit hellokernel_exit(void)
{
        kill_new_kobject();
        debug_print(KERN_INFO "Exit kernel sys_fs!\n");
        return;
}
 
 
module_init(hellokernel_init);
module_exit(hellokernel_exit);
 
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("xueqing");
