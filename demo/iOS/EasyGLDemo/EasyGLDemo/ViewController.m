//
//  Created by Pi on 2021/2/20.
//

#import "ViewController.h"
#import "EasyGLView.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    EasyGLView *easyView = [[EasyGLView alloc] initWithFrame:self.view.bounds];
    [self.view addSubview:easyView];
}

@end
