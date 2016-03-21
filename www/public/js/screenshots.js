$(document).ready(function(){

    function Previous(index)
    {
        var total = $('ul.row li').length;
        
        var newPrevIndex = index - 1; 
        if (newPrevIndex < 0)
        {
            newPrevIndex = total - 1;
        }
        return newPrevIndex;
    }

    function Next(index)
    {
        var total = $('ul.row li').length;
        
        var newNextIndex = index + 1;
        if (newNextIndex >= total)
        {
            newNextIndex = 0;
        }
        return newNextIndex;
    }


   $('li img').on('click',function() {
        var src = $(this).attr('src');
        var img = '<img src="' + src + '" class="img-responsive"/>';

        //Start of new code
        var index = $(this).parent('li').index();                   
        var html = '';
        html += img;
        html += '<div class="modal-footer">';
        html += '<button type="button" class="btn btn-default pull-left previous" href="' + Previous(index) + '"><i class="glyphicon glyphicon-chevron-left" />&nbsp;Précédent</button>';
        html += '<button type="button" class="btn btn-primary next" href="' + Next(index) + '"">Suivant&nbsp;<i class="glyphicon glyphicon-chevron-right" /></button>';
        html += '</div>';
        //End of new code

        $('#myModal').modal();
        $('#myModal').on('shown.bs.modal', function(){
            $('#myModal .modal-body').html(html);
        });
        $('#myModal').on('shown.bs.modal', function(){
            $('#myModal .modal-title').html(src);
        });
        $('#myModal').on('hidden.bs.modal', function(){
            $('#myModal .modal-body').html('');
        });
   });

    $(document).on('click', 'button.btn', function(){
        var index = parseInt($(this).attr('href'));
        index++;
        var src = $('ul.row li:nth-child('+ index +') img').attr('src');
        index--;            
        
        $('.modal-body img').attr('src', src);

        var newPrevIndex = Previous(index);
        var newNextIndex = Next(index);
        
        if($(this).hasClass('previous')){               
            $(this).attr('href', newPrevIndex); 
            $('button.next').attr('href', newNextIndex);
        }else{
            $(this).attr('href', newNextIndex); 
            $('button.previous').attr('href', newPrevIndex);
        }     
        
        return false;
    });
});

